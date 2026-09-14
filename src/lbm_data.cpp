#include "lbm_data.h"
#include <numeric>


LBMData::LBMData(int x_dims, int y_dims, int f_dims) : 
    dens(x_dims, y_dims), u(x_dims, y_dims), f(f_dims, x_dims, y_dims), fstar(f_dims, x_dims, y_dims) {}



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

double LBMData::get_total_density() const {
    return std::accumulate(dens.begin(), dens.end(), 0.0);
}