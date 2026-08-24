#include "lbm_data.h"



LBMData::LBMData(int x_dims, int y_dims) : 
    dens(x_dims, y_dims), u(x_dims, y_dims), f(0, x_dims, y_dims), fstar(0, x_dims, y_dims) {}



void LBMData::set_all_density(double val){
    dens.fill(val);
}

void LBMData::set_all_velocity(const pba::Vector2<double>& val){
    u.fill(val);
}

int LBMData::dimension(int d) const {
    return dens.get_dim(d);
}