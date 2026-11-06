#pragma once

#include <memory>

#include "lbmd2q9.h"

namespace dpa_lbm{

// to do -> move to a dpa_algorithms helper header. Or actually just use a std algorithm
template <typename T, typename Func>
decltype(auto) sum_over_alpha_beta(Func&& func, T initial, int alpha, int beta){
    T result = initial;
    for (int a = 0; a < alpha; a++){
        for (int b = 0; b < beta; b++){
            result += func(a, b);
        }
    }
    return result;
};

class LBM_EquilibriumFunction {
public:
	virtual double solve(
		const LBMLattice& lattice,
		const double dens,
		const pba::Vector2<double>& u,
		const int lattice_point
    ) const;
};

class LBM_EquilibriumIncompressible : public LBM_EquilibriumFunction {
public:
	//LBM_EquilibriumIncompressible() = delete; // must provide a density
	LBM_EquilibriumIncompressible(double rest_density) : _rest_dens(rest_density) {}
	
	double solve(
		const LBMLattice& lattice,
		const double dens,
		const pba::Vector2<double>& u,
		const int lattice_point
    ) const override;

private:
	const double _rest_dens;
};

using feq_ptr = std::shared_ptr<LBM_EquilibriumFunction>;



} // end namespace dpa_lbm