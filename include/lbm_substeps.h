#pragma once


#include "lbm_data.h"
#include "lbmd2q9.h"
#include "bounce_back_halfway.h"
#include "lbm_equilibrium.h"


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

class LBMPartialSolverWithFeq : public LBMPartialSolver{
public:
    explicit LBMPartialSolverWithFeq(LBMLatticePtr lattice, LBMDataPtr data, feq_ptr f_eq_func) : 
        LBMPartialSolver(lattice, data), _feq_func(f_eq_func) {}
    
    
protected:
    feq_ptr _feq_func;
};

class LocalCollision : public LBMPartialSolverWithFeq{
public:
    // A pain to have to inherit the constructor. Makes me want to just do lambdas T.T
    using LBMPartialSolverWithFeq::LBMPartialSolverWithFeq;

    void solve() override;
private:
    void _local_collision(int i, int j, int q);
};

class LocalCollisionWithForce : public LBMPartialSolverWithFeq{
public:
    using LBMPartialSolverWithFeq::LBMPartialSolverWithFeq;
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

class BoundaryZouHeInlet : public LBMPartialSolver{
public:
    BoundaryZouHeInlet(LBMLatticePtr lattice, LBMDataPtr data, double inlet_vel) : 
        LBMPartialSolver(lattice, data), _inlet_velocity(inlet_vel) {}
    void solve() override;
private:
    double _inlet_velocity = 0;
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



