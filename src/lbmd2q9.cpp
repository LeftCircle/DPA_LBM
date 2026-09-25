#include "lbmd2q9.h"



LBMd2q9::LBMd2q9(double dt, double tau, double w0) : _dt(dt) {
    _set_speed_of_sound_from_dx_dt();
    set_tau(tau);
    double visc = _c_s * _c_s * (_tau - _dt * 0.5);
    printf("Shear viscoscity is %f\n", visc);
}

// triggers a lot of changes. Tau, dt, and velocities will be auto adjusted. 
void LBMd2q9::set_dt(double dt) {
    _dt = dt;
    set_tau(_tau);
    _set_speed_of_sound_from_dx_dt();
}

void LBMd2q9::set_tau(double tau) {
    // viscoscity = c_s^2 ( tau - dt / 2), so since visc should always be > 0
    _tau = std::max(_dt / 2.0 + 0.01, tau);
    _dt_over_tau = _dt / tau;
}

void LBMd2q9::set_speed_of_sound(double c_s){
    _c_s = c_s;
    _one_over_cs_squared = 1.0 / (_c_s * _c_s);
    _one_over_cs_fourth = _one_over_cs_squared * _one_over_cs_squared;
}


void LBMd2q9::_set_speed_of_sound_from_dx_dt() {
    _c_s = (1.0 / std::sqrt(3.0)) * (_dx / _dt);
    //printf("c_s set to %f\n", _c_s);
    //_c_s = 1.0;
    _one_over_cs_squared = 1.0 / (_c_s * _c_s);
    _one_over_cs_fourth = _one_over_cs_squared * _one_over_cs_squared;
}


double LBMd2q9::advance(LBMData& data, const double t) const {
    _set_distribution_function_dimensions(data);
    compute_moments(data);
    compute_local_collisions(data);
    propogate_to_neighbors(data);
    return t + _dt;
}

void LBMd2q9::_set_distribution_function_dimensions(LBMData& data) const {
    if (data.n_qs() != N_LATTICE_POSITIONS) [[unlikely]] {
        data.resize_f_dimension(0, N_LATTICE_POSITIONS);
    }
}

void LBMd2q9::compute_moments(LBMData& data) const {
    auto& f = data.f;
    #pragma omp parallel for
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const double* q_block = data.f.data() + data.f.index(0, i, j);
            data.dens(i, j) = std::accumulate(q_block, q_block + N_LATTICE_POSITIONS, 0.0);
            if (data.dens(i, j) == 0){
                data.u(i, j).set(0, 0);
            } else {
                double x = (f(1, i, j) + f(5, i, j) + f(8, i, j) - (f(3,i,j) + f(6,i,j) + f(7,i,j))) / data.dens(i,j); 
                double y = (f(2, i, j) + f(5, i, j) + f(6, i, j) - (f(4,i,j) + f(7,i,j) + f(8,i,j))) / data.dens(i,j);
                data.u(i, j).set(x, y);
            }
        }
    }
}

double LBMd2q9::single_f_equilibrium(
		const double dens,
		const pba::Vector2<double>& u,
		const int i
	) const
{
    double cu = _ci[i] * u;
    double uu = u * u;
    return _weights[i] * dens * (1.0 + cu * _one_over_cs_squared +
        cu * cu * 0.5 * _one_over_cs_fourth - 
        uu * 0.5 * _one_over_cs_squared
    );

}

// I feel like this is the perfect use case for std::transform, but carrying the index that is used all the time is 
// quite frustrating. Should try boost to simplify this. 
void LBMd2q9::compute_local_collisions(LBMData& data) const {
    const int _y = data.dimension(1);
    const int _x = data.dimension(0);
    #pragma omp parallel for
    for (int j = 0; j < _y; j++){
        for (int i = 0; i < _x; i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < N_LATTICE_POSITIONS; q++){
                data.fstar(q, i, j) = data.f(q, i, j) - _dt_over_tau * (data.f(q, i, j) - single_f_equilibrium(dens, u, q));
            }
        }
    }
}

void LBMd2q9::propogate_to_neighbors(LBMData& data) const {
    //boundary_collision(data);
    propogate_all_points(data);
}


void LBMd2q9::boundary_collision(LBMData& data) const {
    const int _y = data.dimension(1);
    const int _x = data.dimension(0);
    auto bound_coll_i = [*this, &data](int x, int y, int q, int p){
        return single_f_equilibrium(p, data.u(x, y), q) + 
                        (data.fstar(q, x, y) - 
                        single_f_equilibrium(data.dens(x, y), data.u(x, y), q));
    };
    // Only having boundary condition with pressure gradient on sides
    float inlet_pressure = 2.0;
    float outlet_pressure = 1.0;
    #pragma omp parallel for num_threads(4)
    for (int j = 0; j < _y; j++){
        // left side
        data.fstar(1, 0, j) = bound_coll_i(_x - 2, j, 1, inlet_pressure);
        data.fstar(5, 0, j) = bound_coll_i(_x - 2, j, 5, inlet_pressure);
        data.fstar(8, 0, j) = bound_coll_i(_x - 2, j, 8, inlet_pressure);

        // right side
        data.fstar(3, _x - 1, j) = bound_coll_i(1, j, 3, outlet_pressure);
        data.fstar(6, _x - 1, j) = bound_coll_i(1, j, 6, outlet_pressure);
        data.fstar(7, _x - 1, j) = bound_coll_i(1, j, 7, outlet_pressure);
    }
}

void LBMd2q9::propogate_all_points(LBMData& data) const {
    const int _y = data.dimension(1);
    const int _x = data.dimension(0);
    const auto& bounds = data.get_bounds();
    #pragma omp parallel for
    for (int j = 0; j < _y; j++){
        for (int i = 0; i < _x; i++){
            for (int q = 0; q < N_LATTICE_POSITIONS; q++){
                if (bounds.is_blocked(i, j, q)) continue;

                int ni = (i + static_cast<int>(_ci[q].X()) + data.dimension(0)) % data.dimension(0);
                int nj = (j + static_cast<int>(_ci[q].Y()) + data.dimension(1)) % data.dimension(1);
                if (bounds.is_blocked(ni, nj, q)){
                    data.set_f(i, j, get_fi_minus(q), data.get_fstar(i, j, q));
                } else {
                    data.f(q, ni, nj) = data.fstar(q, i, j);
                }
            }
        }
    }
}

void LBMd2q9::set_to_equilibrium(LBMData& data) const {
   const int _y = data.dimension(1);
    const int _x = data.dimension(0);
    #pragma omp parallel for
    for (int j = 0; j < _y; j++){
        for (int i = 0; i < _x; i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < 9; q++){
                data.f(q, i, j) = single_f_equilibrium(dens, u, q);
            }   
        }
    }
}

double LBMd2q9::get_shear_viscoscity() const {
    return _c_s * _c_s * (_tau - _dt * 0.5);
}

double LBMd2q9::compute_max_mach_number(const pba::Vector2<double>& max_vel) const {
    // mach is u / c_s, so get max speed over c_s
    return max_vel.magnitude() / _c_s;
}

double LBMd2q9::estimate_reynolds_number(const pba::Vector2<double>& max_vel, float macroscopic_scale) const{
    return max_vel.magnitude() * macroscopic_scale / get_shear_viscoscity();
}