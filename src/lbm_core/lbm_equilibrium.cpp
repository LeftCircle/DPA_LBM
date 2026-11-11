#include "lbm_equilibrium.h"


using namespace dpa_lbm;


double LBM_EquilibriumFunction::solve(
    const LBMLattice& lattice,
    const double dens,
    const pba::Vector2<double>& u,
    const int lattice_point
) const {
    const auto c = lattice.get_velocity(lattice_point);
    double cu = c * u;
    //double uu = u * u;
    const auto wi = lattice.get_weight(lattice_point);
    //const auto cu_accum_func = 
    auto accum = sum_over_alpha_beta(
        [&c, dens, &u, cs = lattice.c_s()](const int alpha, const int beta){
        auto uab = u[alpha] * u[beta];
        auto cab = c[alpha] * c[beta];
        auto chron = alpha == beta ? 1 : 0;
        return uab * (cab - cs * cs * chron);
        },
        0.0, 2, 2
    );
    return wi * dens * (1.0 + cu * lattice.one_over_cs_squared() +
           accum * 0.5 * lattice.one_over_cs_fourth()
    );
    // This should also work
    // const auto c = lattice.get_velocity(lattice_point);
    // const double cu = c * u;
    // const double uu = u * u;

    // return lattice.get_weight(lattice_point) * dens * (
    //     1.0
    //     + cu * lattice.one_over_cs_squared()
    //     + 0.5 * cu * cu * lattice.one_over_cs_fourth()
    //     - 0.5 * uu * lattice.one_over_cs_squared()
    // );
}

double LBM_EquilibriumIncompressible::solve(
    const LBMLattice& lattice,
    const double dens,
    const pba::Vector2<double>& u,
    const int lattice_point
) const {
    const auto c = lattice.get_velocity(lattice_point);
    double cu = c * u;
    double uu = u * u;
    const auto wi = lattice.get_weight(lattice_point);
    //const auto cu_accum_func = 
    auto accum = sum_over_alpha_beta(
        [&c, dens, &u, cs = lattice.c_s()](const int alpha, const int beta){
        auto uab = u[alpha] * u[beta];
        auto cab = c[alpha] * c[beta];
        auto chron = alpha == beta ? 1 : 0;
        return uab * (cab - cs * cs * chron);
        },
        0.0, 2, 2
    );

    return wi * dens + wi * _rest_dens * (cu * lattice.one_over_cs_squared() +
           accum * 0.5 * lattice.one_over_cs_fourth()
    );
}

