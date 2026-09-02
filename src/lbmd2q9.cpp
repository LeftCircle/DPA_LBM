#include "lbmd2q9.h"



double LBMd2q9::advance(LBMData& data, const double t, const double dt) {
    _set_distribution_function_dimensions(data);
    compute_moments(data);
    compute_local_collisions(data, dt);
    propogate_to_neighbors(data);
    return t + dt;
}

void LBMd2q9::_set_distribution_function_dimensions(LBMData& data){
    if (data.n_qs() != 9){
        data.resize_f_dimension(0, 9);
    }
}

void LBMd2q9::compute_moments(LBMData& data) {
    auto& f = data.f;
    auto& fs = data.fstar;
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const double* q_block = data.f.data() + data.f.index(0, i, j);
            data.dens(i, j) = std::accumulate(q_block, q_block + 9, 0.0);
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
		const double one_over_cs_squared,
		const double one_over_cs_fourth,
		const int i
	)
{
    double cu = _ci[i] * u;
    double uu = u * u;
    return _weights[i] * dens * (1.0 + cu * one_over_cs_squared +
        cu * cu * 0.5 * one_over_cs_fourth - 
        uu * 0.5 * one_over_cs_squared
    );

}


// I feel like this is the perfect use case for std::transform, but carrying the index that is used all the time is 
// quite frustrating. Should try boost to simplify this. 
void LBMd2q9::compute_local_collisions(LBMData& data, const double dt) {
    const double dt_over_tau = dt / data.get_tau();
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < 9; q++){
                data.fstar(q, i, j) = data.f(q, i, j) * (1.0 - dt_over_tau) + 
                                    single_f_equilibrium(dens, u, data.get_one_over_cs_squaerd(), data.get_one_over_cs_fourth(), q) * 
                                    dt_over_tau;
            }
        }
    }
}

void LBMd2q9::propogate_to_neighbors(LBMData& data) {
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

void LBMd2q9::set_to_equilibrium(LBMData& data, const double dt) {
    const double dt_over_tau = dt / data.get_tau();
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            const auto& dens = data.dens(i, j);
            const auto& u = data.u(i, j);
            for (int q = 0; q < 9; q++){
                data.f(q, i, j) = single_f_equilibrium(dens, u, data.get_one_over_cs_squaerd(), data.get_one_over_cs_fourth(), q);
            }   
        }
    }
}