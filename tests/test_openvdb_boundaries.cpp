#include "catch_helpers.h"
#include <openvdb/openvdb.h>



#include "lbm_data.h"
#include "lbmd2q9.h"
#include "bounce_back_halfway.h"



TEST_CASE("test setting and finding border cells"){
    openvdb::initialize();

    int n_lattice_points = 9;
    LBMData data(10, 10, n_lattice_points);
    data.get_bounds().block(5, 5);
    
    // REQUIRE ALL LATTICE POINTS IN THE BOUNDARY ARE MARKED AS BOUNDS
    for (int i = 0; i < n_lattice_points; i++){
        REQUIRE(data.get_bounds().is_blocked(5, 5, i));
    }

    data.get_bounds().block(0, 0, 1);
    REQUIRE(data.get_bounds().is_blocked(0, 0, 1));
    REQUIRE(!data.get_bounds().is_blocked(0, 0, 0));
}


TEST_CASE("Test fs for a vertical boundary with bounce back halfway"){
    LBMData data(3, 3, 9);

    // set 1, 5 and 8 to 1, have them step into a bound, and confirm
    // that 3, 6, 7 are now -1. 0 should remain 0

    data.get_bounds().block(2, 0);
    data.get_bounds().block(2, 1);
    data.get_bounds().block(2, 2);
    data.set_f(1, 1, 1, 1);
    data.set_f(1, 1, 5, 1);
    data.set_f(1, 1, 8, 1);

    LBMd2q9 solver(1, 1);
    solver.advance(data, 1);

    REQUIRE(data.get_f(1, 1, 3) > 0);
    REQUIRE(data.get_f(1, 1, 6) > 0);
    REQUIRE(data.get_f(1, 1, 7) > 0);
}

TEST_CASE(" test blocked nodes don't stream"){
    LBMData data(3, 3, 9);

    // set 1, 5 and 8 to 1, have them step into a bound, and confirm
    // that 3, 6, 7 are now -1. 0 should remain 0

    data.get_bounds().block(2, 0);
    data.get_bounds().block(2, 1);
    data.get_bounds().block(3, 0);
    data.set_f(2, 0, 3, 1);
    data.set_f(2, 1, 6, 1);
    data.set_f(2, 2, 7, 1);

    LBMd2q9 solver(1, 1);
    solver.advance(data, 1);

    REQUIRE(data.get_f(1, 0, 3) == 0);
    REQUIRE(data.get_f(1, 1, 6) == 0);
    REQUIRE(data.get_f(1, 2, 7) == 0);
}