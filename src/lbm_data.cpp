#include "lbm_data.h"



LBMData::LBMData(int x_dims, int y_dims, int f_dims, double c_s, double tau) : 
    dens(x_dims, y_dims), u(x_dims, y_dims), f(f_dims, x_dims, y_dims), fstar(f_dims, x_dims, y_dims),
    _c_s(c_s), _one_over_cs_squared(1.0 / (c_s * c_s)), _one_over_cs_fourth(1.0 / (_one_over_cs_squared * _one_over_cs_squared)),
    _tau(tau) {}



void LBMData::set_all_density(double val){
    dens.fill(val);
}

void LBMData::set_all_velocity(const pba::Vector2<double>& val){
    u.fill(val);
}

void LBMData::set_all_f(double val){
    f.fill(val);
}

int LBMData::dimension(int d) const {
    return dens.get_dim(d);
}