
#include <iostream>
#include "lbm_viewer2D.h"
#include "lbm_substeps.h"

using namespace dpa_lbm;

int main(int argc, char** argv){
    
    int w = 1920 / 4;
    int h = 1080 / 4;
    std::shared_ptr<LBMData> data = std::make_shared<LBMData>(w, h, LBMd2q9::N_LATTICE_POSITIONS);
    std::shared_ptr<LBMd2q9> d2q9_lattice = std::make_shared<LBMd2q9>(1.0, 1.0);
    d2q9_lattice->set_speed_of_sound(1.0 / sqrt(3.0));
    d2q9_lattice->set_tau(0.9);
    double constant_inlet_vel = 0.1;
    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){
            data->dens(i, j) = 1;
            data->set_velocity(i, j, {constant_inlet_vel, 0});
        }
    }
    
    // for (int j = (2*h / 5); j < (3 * h / 5); j++){
    //     for (int i = (2*w / 5); i < (3 * w / 5); i++){
    //         data->dens(i, j) = 1.01;
    //     }
    // }
    // float large_val = 1.0;
    // data->dens(w / 2, h / 2) = large_val;
    // data->dens(w / 4, h / 4) = large_val;
    // data->dens(w / 4, 3 * h / 4) = large_val;
    // data->dens(3 * w / 4, h / 4) = large_val;
    // data->dens(3 * w / 4, 3 * h / 4) = large_val;


    // // left side forces
    // for (int j = 0; j < data->dimension(1); j++){
    //     data->set_acceleration(0, j, pba::vec2d(0.001, 0));
    // }
    
    // // Block a square in the middle
    // for (int j = (2*h / 5); j < (3 * h / 5); j++){
    //     for (int i = (19*w / 40); i < (21 * w / 40); i++){
    //         data->get_bounds().block(i, j);
    //     }
    // }

    // Create the solver system
    
    auto d = std::shared_ptr<LBMData>(data);
    auto lattice = std::shared_ptr<LBMd2q9>(d2q9_lattice);
    //auto feq = std::make_shared<LBM_EquilibriumIncompressible>(0.8);
    auto feq = std::make_shared<LBM_EquilibriumFunction>();
    //auto local_coll = std::make_shared<LocalCollisionWithForce>(lattice, d, feq);
    auto local_coll = std::make_shared<LocalCollision>(lattice, d, feq);
    auto periodic = std::make_shared<PeriodicPropagation>(lattice, d);
    auto boundary_cond = std::make_shared<BoundaryZouHeInlet>(lattice, d, constant_inlet_vel);
    auto bounds = std::make_shared<BounceBackHalfwaySolver>(lattice, d);
    auto moment_update = std::make_shared<MomentUpdatePeriodicD2Q9>(lattice, d);


    auto sys = std::make_shared<SolverSystem>(local_coll, periodic, boundary_cond, bounds, moment_update);
    //auto sys = std::make_shared<SolverSystem>(local_coll, periodic, moment_update);

    // set gravity forces
    // for (int j = 0; j < data->dimension(1); j++){
    //     for (int i = 0; i < data->dimension(0); i++){
    //         data->set_acceleration(i, j, pba::vec2d(0, -0.0025));
    //     }
    // }

    // Top and bottom walls
    for (int i = 0; i < w; i++){
        bounds->get_bounds().block(i, 0);
        bounds->get_bounds().block(i, 1);
        bounds->get_bounds().block(i, h - 1);
        bounds->get_bounds().block(i, h - 2);
    }

    // small barrier to bounce around with grav
    for (int j = (9*h / 40); j < (11 * h / 40); j++){
        for (int i = (15*w / 40); i < (25 * w / 40); i++){
            bounds->get_bounds().block(i, j);
        }
    }

    d2q9_lattice->set_to_equilibrium(*data);

    
    LBMViewer2D viewer(data, d2q9_lattice, sys);
    viewer.bounds = &bounds->get_bounds();
    viewer.start_viewer();

    return 0;
}

