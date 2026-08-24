#include "catch_helpers.h"

#include "lbm_data.h"


TEST_CASE("Test LBM info propogates"){

    // Create a 3x3 grid of LBM data. 
    // Set density to 1 and u to zero for all. 
    // Advance dt and assert that all are the same.  
    LBMData lbm_data(3, 3);
    lbm_data.set_all_density(1);
    lbm_data.set_all_velocity(0, 0);

    LBMd2q9.advance(lbm_data, 1.0);
    
    bool is_zero = true;
    // Loop over each grid point and check if nonzero
    for (int j = 0; j < lbm_data.dimension(1); j++){
        for (int i = 0; i < lbm_data.dimension(0); i++){
            if (lbm_data.velocity(i, j) != pba::Vector2<double>(0, 0)){
                is_zero = false;
                break;
            }
        }
    }
    REQUIRE(is_zero);

    // Set u of the first element to (1, 0). Check to see that
    // that the second lattice point has a u.x > 0
    lbm_data.set_velocity(0, 0, pba::Vector2(1.0, 0.0));
    lbm_advance_d2q9(lbm_data, 1.0);
    REQUIRE(lbm_data.velocity(0, 0).X() == 1.0);
    REQUIRE(lbm_data.velocity(1, 0).X() > 0.0);
}

