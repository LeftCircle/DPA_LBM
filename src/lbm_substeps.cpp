#include "lbm_substeps.h"



namespace dpa_lbm{




void LocalCollision::solve() {
    _data.for_each(_local_collision);
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

double LBMd2q9::compute_force_source_term(
    const pba::vec2d& F,
    const pba::vec2d& u,
    int i
) const {
    double s = 0;
    for (int alpha = 0; alpha < 2; alpha++){
        for (int beta = 0; beta < 2; beta++){
            auto t1 = (1.0 - _dt_over_tau * 0.5);
            auto t2 = _ci[i][alpha] * _one_over_cs_squared;
            auto t3 = _ci[i][alpha] * _ci[i][beta];
            t3 = alpha == beta ? t3 - _c_s * _c_s : t3;
            t3 *= u[beta] * _one_over_cs_fourth;
            s += t1 * _weights[i] * (t2 - t3) * F[alpha];
        }
    }
    return s;
}

double LBMd2q9::compute_collision_operator(
    LBMData& data,
    int x, 
    int y,
    int i
) const{
    auto fstar = single_f_equilibrium(data.dens(x, y), data.u(x, y), i);
    return -1.0 / _tau * (data.get_f(x, y, i) - fstar);
}

double LBMd2q9::local_collision_and_source(
    LBMData& data,
    int x, 
    int y,
    int q
) const {
    auto collision_op = compute_collision_operator(data, x, y, q);
    auto F = data.get_acceleration(x, y) * data.dens(x, y);
    auto source = compute_force_source_term(F, data.u(x, y), q);
    return data.get_f(x, y, q) + (collision_op + source) * _dt;
}

void LBMd2q9::compute_local_collisions(LBMData& data) const {
    const int _y = data.dimension(1);
    const int _x = data.dimension(0);
    #pragma omp parallel for
    for (int j = 0; j < _y; j++){
        for (int i = 0; i < _x; i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < N_LATTICE_POSITIONS; q++){
                //data.fstar(q, i, j) = data.f(q, i, j) - _dt_over_tau * (data.f(q, i, j) - single_f_equilibrium(dens, u, q));
                data.fstar(q, i, j) = local_collision_and_source(data, i, j, q);
            }
        }
    }
}



} // end namespace dpa_lbm