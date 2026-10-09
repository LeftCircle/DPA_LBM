#include "lbm_data.h"
#include <numeric>


LBMData::LBMData(int x_dims, int y_dims, int f_dims) : 
    dens(x_dims, y_dims), u(x_dims, y_dims), f(f_dims, x_dims, y_dims),
    fstar(f_dims, x_dims, y_dims), _bounds(x_dims, y_dims),
    _Force(x_dims, y_dims) {}


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

const pba::Vector2<double>& LBMData::get_max_u() const {
    const auto max_vel = std::max_element(u.begin(), u.end(), [](const pba::Vector2<double>& l, const pba::Vector2<double>& r){
        return l.magnitude_squared() < r.magnitude_squared();
    });
    if (max_vel == u.end()){
        return pba::vec2d(0, 0);
    }
    return *max_vel;

}

double LBMData::get_vorticity(int x, int y) const {
    // vort is du_y / dx - du_x / dy
    // So we need the neighbor positions
    y = std::clamp(y, 1, dimension(1) - 2);
    x = std::clamp(x, 1, dimension(0) - 2);
    auto up_vel = velocity(x, y + 1);
    auto below_vel = velocity(x, y - 1);
    auto left_vel = velocity(x - 1, y);
    auto right_vel = velocity(x + 1, y);

    double duy_over_dx = (right_vel[1] - left_vel[1]) / 2.0;
    double dux_over_dy = (up_vel[0] - below_vel[0]) / 2.0;
    return duy_over_dx - dux_over_dy;

}