#include "lbmd2q9.h"



double LBMd2q9::advance(LBMData& data, const double t, const double dt) {
    _set_distribution_function_dimensions(data);
    compute_moments(data);
    compute_equilibrium(data);
    compute_local_collisions(data);
    propogate_to_neighbors(data);
    return t + dt;
}

void LBMd2q9::_set_distribution_function_dimensions(LBMData& data){
    if (data.dimension(0) != 9){
        data.resize_dimension(0, 9);
    }
}

void LBMd2q9::compute_moments(LBMData& data) {
    for (int j = 0; j < data.dimension(1); j++){
        for (int i = 0; i < data.dimension(0); i++){
            int val = 0;
            for (int q = 0; q < 9; q++){
                val += data.f(q, i, j);
            }
            data.dens(i, j) = val;
        }
    }
    // this is the perfect functional algorithm time!
    // auto f = data.f;
    // std::transform(data.dens.begin(), data.dens.end(), data.dens.begin(), 
    //     [&f](double x){
    //         // f_i is set up as [q][Nx][Ny], so we need the iterator at [0][x][y]
    //         // which means we also need to know which [x][y] we are at in the transform....
    //         x = std::accumulate()
    //     }
    // );
}