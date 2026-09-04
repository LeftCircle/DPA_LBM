#include "lbmd2q9.h"



LBMd2q9::LBMd2q9(double dt, double tau, double w0) : _dt(dt) {
    _set_speed_of_sound_from_dx_dt();
    set_tau(tau);
}

// triggers a lot of changes. Tau, dt, and velocities will be auto adjusted. 
void LBMd2q9::set_dt(double dt) {
    
    // reset tau based on new dt
    set_tau(_tau);
}

void LBMd2q9::set_tau(double tau) {
    // viscoscity = c_s^2 ( tau - dt / 2), so since visc should always be > 0
    _tau = std::max(_dt / 2.0, tau);
    _dt_over_tau = _dt / tau;
}


void LBMd2q9::_set_speed_of_sound_from_dx_dt() {
    double dx_o_dt = _dx / _dt;
    _c_s = 1.0 / 3.0 * dx_o_dt * dx_o_dt;
    _one_over_cs_squared = 1.0 / (_c_s * _c_s);
    _one_over_cs_fourth = _one_over_cs_squared * _one_over_cs_squared;
}

// We have to uphold w_i * c_ia = 0 and w_i * c_ia * c_ib = chrondelta_ab * cs^2
// and dt * c_ia = 1.0;
void LBMd2q9::_set_local_velocities_based_on_dt_and_weights() {
    // maybe just try and see if conditions are met and print out if so?
    
    
}


double LBMd2q9::advance(LBMData& data, const double t) const {
    _set_distribution_function_dimensions(data);
    compute_moments(data);
    compute_local_collisions(data);
    propogate_to_neighbors(data);
    return t + _dt;
}

void LBMd2q9::_set_distribution_function_dimensions(LBMData& data) const {
    if (data.n_qs() != _n_lattice_positions){
        data.resize_f_dimension(0, _n_lattice_positions);
    }
}

void LBMd2q9::compute_moments(LBMData& data) const {
    auto& f = data.f;
    auto& fs = data.fstar;
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const double* q_block = data.f.data() + data.f.index(0, i, j);
            data.dens(i, j) = std::accumulate(q_block, q_block + _n_lattice_positions, 0.0);
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
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < _n_lattice_positions; q++){
                data.fstar(q, i, j) = data.f(q, i, j) - _dt_over_tau * (data.f(q, i, j) - single_f_equilibrium(dens, u, q));
            }
        }
    }
}

void LBMd2q9::propogate_to_neighbors(LBMData& data) const {
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            for (int q = 0; q < 9; q++){
                int ni = (i + static_cast<int>(_ci[q].X()) + data.dimension(0)) % data.dimension(0);
                int nj = (j + static_cast<int>(_ci[q].Y()) + data.dimension(1)) % data.dimension(1);
                data.f(q, ni, nj) = data.fstar(q, i, j);
            }
        }
    }
}

void LBMd2q9::set_to_equilibrium(LBMData& data) const {
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < 9; q++){
                data.f(q, i, j) = single_f_equilibrium(dens, u, q);
            }   
        }
    }
}