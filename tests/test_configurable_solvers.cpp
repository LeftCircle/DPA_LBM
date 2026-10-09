#include "catch_helpers.h"


#include "lbmd2q9.h"



TEST_CASE("test configurable lbm solver"){
    // LBM solvers have a few different parts that can be 
    // added/removed such as boundary conditions and forces. 
    
    // One thing to note is that when there are forces present, 
    // the equations for building the velocity moments change

    // so we could have someething like
    // auto moment_func = make_force_free_moment_func();
    // auto boundary_func = [](){};
    // auto local_collision_func = [](){};
    // auto propogation = [](){};
    // auto force_computation = [](){};
    // auto force_discretization = [](){};
    // LBMSolverSystem system(
    //     moment_func,
    //     boundary_func,
    //     local_collision_func,
    //     propogation,
    //     force_computation,
    //     force_discretization
    // );
    
    // The solver system should now have an array of functions that it can use?



}



