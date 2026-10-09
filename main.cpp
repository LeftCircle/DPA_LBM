
#include <iostream>
#include "lbm_viewer2D.h"

using namespace dpalbm;

int main(int argc, char** argv){
    
    int w = 1920 / 8;
    int h = 1080 / 8;
    std::shared_ptr<LBMData> data = std::make_shared<LBMData>(w, h, LBMd2q9::N_LATTICE_POSITIONS);
    std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 1.0);
    solver->set_speed_of_sound(1.0 / sqrt(3.0));
    solver->set_tau(0.95);
    
    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){
            data->dens(i, j) = 1;
        }
    }
    
    for (int j = (2*h / 5); j < (3 * h / 5); j++){
        for (int i = (2*w / 5); i < (3 * w / 5); i++){
            data->dens(i, j) = 1.02;
        }
    }
    float large_val = 1.0;
    data->dens(w / 2, h / 2) = large_val;
    data->dens(w / 4, h / 4) = large_val;
    data->dens(w / 4, 3 * h / 4) = large_val;
    data->dens(3 * w / 4, h / 4) = large_val;
    data->dens(3 * w / 4, 3 * h / 4) = large_val;


    // // left side forces
    // for (int j = 0; j < data->dimension(1); j++){
    //     data->set_force(0, j, pba::vec2d(0.001, 0));
    // }
    
    // // Block a square in the middle
    // for (int j = (2*h / 5); j < (3 * h / 5); j++){
    //     for (int i = (19*w / 40); i < (21 * w / 40); i++){
    //         data->get_bounds().block(i, j);
    //     }
    // }


    // set gravity forces
    for (int j = 0; j < data->dimension(1); j++){
        for (int i = 0; i < data->dimension(0); i++){
            data->set_force(i, j, pba::vec2d(0, -0.0025));
        }
    }

    // small barrier to bounce around with grav
    for (int j = (9*h / 40); j < (11 * h / 40); j++){
        for (int i = (15*w / 40); i < (25 * w / 40); i++){
            data->get_bounds().block(i, j);
        }
    }

    // Top and bottom walls
    for (int i = 0; i < w; i++){
        data->get_bounds().block(i, 0);
        data->get_bounds().block(i, 1);
        data->get_bounds().block(i, h - 1);
        data->get_bounds().block(i, h - 2);
    }

    solver->set_to_equilibrium(*data);

    
    LBMViewer2D viewer(data, solver);
    //viewer.set_min_color(Color(0, 0, 1));
    //viewer.set_max_color(Color(1, 0, 0));
    viewer.start_viewer();

    return 0;
}
