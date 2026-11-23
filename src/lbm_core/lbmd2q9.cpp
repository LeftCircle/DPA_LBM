#include "lbmd2q9.h"

#include <iostream>
#include <fstream>

namespace dpa_lbm{

LBMLattice::LBMLattice(double dt, double tau) : _dt(dt) {
    _set_speed_of_sound_from_dx_dt();
    set_tau(tau);
}

LBMd2q9::LBMd2q9(double dt, double tau, double w0) : LBMLattice(dt, tau) {
    _n_lattice_positions = 9;
    _set_speed_of_sound_from_dx_dt();
    set_tau(tau);
    double visc = _c_s * _c_s * (_tau - _dt * 0.5);
    printf("Shear viscoscity is %f\n", visc);
    //std::ofstream out_mass("./build/output/mass_and_momentum.txt", std::ios::trunc);
    //out_mass << "";
}

// triggers a lot of changes. Tau, dt, and velocities will be auto adjusted. 
void LBMLattice::set_dt(double dt) {
    _dt = dt;
    set_tau(_tau);
    _set_speed_of_sound_from_dx_dt();
}

void LBMLattice::set_tau(double tau) {
    // viscoscity = c_s^2 ( tau - dt / 2), so since visc should always be > 0
    _tau = std::max(_dt / 2.0 + 0.01, tau);
    _dt_over_tau = _dt / _tau;
}

void LBMd2q9::set_speed_of_sound(double c_s){
    _c_s = c_s;
    _one_over_cs_squared = 1.0 / (_c_s * _c_s);
    _one_over_cs_fourth = _one_over_cs_squared * _one_over_cs_squared;
}


void LBMLattice::_set_speed_of_sound_from_dx_dt() { 
    _c_s = (1.0 / std::sqrt(3.0)) * (_dx / _dt);
    _one_over_cs_squared = 1.0 / (_c_s * _c_s);
    _one_over_cs_fourth = _one_over_cs_squared * _one_over_cs_squared;
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


}