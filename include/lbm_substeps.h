#pragma once

#include <memory>
#include <vector>

#include "lbm_data.h"
#include "lbmd2q9.h"



namespace dpa_lbm{

// LBM Partial Solvers are basically functors that take in LBMData and a lattice (ie d2q9)
class LBMPartialSolver {
public:
    LBMPartialSolver(LBMLatticePtr lattice, LBMDataPtr data) : 
        _lattice(std::move(lattice)), _data(std::move(data)) {}

    virtual void solve() = 0;

protected:
    LBMLatticePtr _lattice;
    LBMDataPtr _data;
};

using LBMPartialSolverPtr = std::shared_ptr<LBMPartialSolver>;

class LocalCollision : public LBMPartialSolver{
public:
    void solve() override;
private:

};



class SolverSystem{
public:
    template <typename... SolverType>
    explicit SolverSystem(std::shared_ptr<SolverType>... solvers) :
        _solvers{std::move(solvers)...}
    {
        static_assert(
            (std::is_base_of_v<LBMPartialSolver, SolverType> && ...), 
            "Each solver must inherit from LBMPartialSolver"
        );
    }


private:
    std::vector<LBMPartialSolverPtr> _solvers;
};





} // end namespace pba_lbm



