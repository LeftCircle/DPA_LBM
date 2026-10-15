#pragma once

#include <memory>

#include "lbm_data.h"
#include "lbmd2q9.h"



namespace pba_lbm{

// LBM Partial Solvers are basically functors that take in LBMData and a lattice (ie d2q9)
class LBMPartialSolver {
public:
    LBMPartialSolver(LBMLatticePtr lattice, LBMDataPtr data) : 
        _lattice(std::move(lattice)), _data(std::move(data)) {}

private:
    LBMLatticePtr _lattice;
    LBMDataPtr _data;

}






} // end namespace pba_lbm



