#include "catch_helpers.h"

#include <cmath>


#include "lbm_data.h"
#include "lbmd2q9.h"


TEST_CASE("Test LBM info propogates"){

    // Create a 3x3 grid of LBM data. 
    // Set density to 1 and u to zero for all. 
    // Advance dt and assert that all are the same.  
    double speed_of_sound = 1.0;
    double tau = 1.0;
    LBMData lbm_data(3, 3, 9, speed_of_sound, tau);
    lbm_data.set_all_density(1.0);
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            REQUIRE(lbm_data.dens(i, j) == 1.0);
        }
    }

    lbm_data.set_all_velocity(0, 0);
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            REQUIRE(lbm_data.u(i, j) == pba::vec2d(0, 0));
        }
    }

    bool is_zero = true;
    // Loop over each grid point and check if nonzero
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            if (std::abs(lbm_data.velocity(i, j).magnitude_squared()) > 0.001){
                is_zero = false;
                break;
            }
        }
    }
    REQUIRE(is_zero);

    LBMd2q9::set_to_equilibrium(lbm_data, 1.0);
    
    // confirm that equilibrium is not nan
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            for(int q = 0; q < 9; q++){
                REQUIRE(!std::isnan(lbm_data.f(q, i, j)));
            }
        }
    }
    
    LBMd2q9::advance(lbm_data, 0.0, 1.0);
    
    // Loop over each grid point and check if nonzero
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            if (std::abs(lbm_data.velocity(i, j).magnitude_squared()) > 0.001){
                is_zero = false;
                printf("(%i, %i) Nonzero u of %f, %f, %f \n", i, j, lbm_data.velocity(i, j).X(), lbm_data.velocity(i, j).Y(), lbm_data.velocity(i, j).magnitude_squared());
                break;
            }
        }
    }
    REQUIRE(is_zero);

    // Set u of the first element to (1, 0). Check to see that
    // that the second lattice point has a u.x > 0
    //lbm_data.set_velocity(0, 0, pba::Vector2(1.0, 0.0));
    lbm_data.f(1, 0, 0) = lbm_data.f(1, 0, 0) + 10.0;
    LBMd2q9::advance(lbm_data, 1.0, 1.0);
    REQUIRE(lbm_data.velocity(0, 0).X() != 0.0);
}

TEST_CASE("test hand solution"){
    double speed_of_sound = 10;
    LBMData lbm_data(3, 3, 9, speed_of_sound, 3.0);
    lbm_data.set_all_density(1.0);
    lbm_data.set_all_f(0);
    lbm_data.f(1, 1, 1) = 3.0;
    REQUIRE(lbm_data.f(1, 1, 1) == (3.0));
    
    LBMd2q9::compute_moments(lbm_data);
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            if (i == 1 && j == 1){
                REQUIRE(lbm_data.velocity(i, j).X() > 0);
            } else{
                REQUIRE(lbm_data.velocity(i, j) == pba::Vector2<double>(0.0, 0.0));
                REQUIRE(lbm_data.dens(i, j) == 0);
            }
        }
    }
    
    
    LBMd2q9::advance(lbm_data, 0.0, 1.0);
    int n_nonzeros = 0;
    // Print all f values
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            for(int q = 0; q < 9; q++){
                //std::cout << "f(" << q << ", " << i << ", " << j << ") = " << lbm_data.f(q, i, j) << std::endl;
                if (lbm_data.f(q, i, j) != 0){
                    n_nonzeros += 1;
                }
            }
        }
    }
    REQUIRE(n_nonzeros == 9);
}
