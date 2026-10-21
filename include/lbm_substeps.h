#pragma once

#include <memory>
#include <vector>
#include <numeric>

#include "lbm_data.h"
#include "lbmd2q9.h"
#include "bounce_back_halfway.h"



namespace dpa_lbm{

double single_f_equilibrium(
    const LBMLattice& lattice,
    const double dens,
    const pba::Vector2<double>& u,
    const int lattice_point
);

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
    // A pain to have to inherit the constructor. Makes me want to just do lambdas T.T
    using LBMPartialSolver::LBMPartialSolver;

    void solve() override;
private:
    void _local_collision(int i, int j, int q);
};

class LocalCollisionWithForce : public LBMPartialSolver{
public:
    using LBMPartialSolver::LBMPartialSolver;
    void solve() override;
private:
    double _compute_force_source_term(
        const pba::vec2d& F,
        const pba::vec2d& u,
        int i
    ) const;

};

class PeriodicPropagation : public LBMPartialSolver{
public:
    using LBMPartialSolver::LBMPartialSolver;
    void solve() override;
};

class MomentUpdatePeriodicD2Q9 : public LBMPartialSolver{
public:
    using LBMPartialSolver::LBMPartialSolver;
    void solve() override;
};

class BounceBackHalfwaySolver : public LBMPartialSolver{
public:
    BounceBackHalfwaySolver(LBMLatticePtr lattice, LBMDataPtr data) : LBMPartialSolver(lattice, data),
            _bounds(data->dimension(0), data->dimension(1)) {}
    void solve() override;
    
    const BounceBackHalfway& get_bounds() const { return _bounds; }
	BounceBackHalfway& get_bounds() { return _bounds; }

private:
    BounceBackHalfway _bounds;
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

    void solve();


private:
    std::vector<LBMPartialSolverPtr> _solvers;
};





} // end namespace pba_lbm



