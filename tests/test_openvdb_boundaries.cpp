#include "catch_helpers.h"
#include <openvdb/openvdb.h>



#include "lbm_data.h"
#include "lbmd2q9.h"





TEST_CASE("test setting and finding border cells"){
    openvdb::initialize();

    int n_lattice_points = 9;
    LBMData data(10, 10, n_lattice_points);
    data.set_boundary(5, 5);
    
    // REQUIRE ALL LATTICE POINTS IN THE BOUNDARY ARE MARKED AS BOUNDS
    for (int i = 0; i < n_lattice_points; i++){
        REQUIRE(data.is_bound(5, 5, i));
    }

    data.set_boundary(0, 0, 1);
    REQUIRE(data.is_bound(0, 0, 1));
    REQUIRE(!data.is_bound(5, 5, 0));
}