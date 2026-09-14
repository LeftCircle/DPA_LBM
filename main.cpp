
#include <iostream>
#include "lbm_viewer2D.h"

using namespace dpalbm;

int main(int argc, char** argv){
    
    int w = 1920 / 2;
    int h = 1080 / 2;
    std::shared_ptr<LBMData> data = std::make_shared<LBMData>(w, h, LBMd2q9::N_LATTICE_POSITIONS);
    std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 10.0);
    for (int j = (2*h / 5); j < (3 * h / 5); j++){
        for (int i = (2*w / 5); i < (3 * w / 5); i++){
            data->dens(i, j) = 10.1;
        }
    }
    //data->dens(w / 2, h / 2) = 10;

    solver->set_to_equilibrium(*data);

    
    LBMViewer2D viewer(data, solver);
    viewer.start_viewer();

    return 0;
}
