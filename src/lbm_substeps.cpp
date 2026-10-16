#include "lbm_substeps.h"



namespace dpa_lbm{


void SolverSystem::solve(){
    for (const auto& solver : _solvers){
        solver->solve();
    }
}

double single_f_equilibrium(
    const LBMLattice& lattice,
    const double dens,
    const pba::Vector2<double>& u,
    const int lattice_point
){
    double cu = lattice.get_velocity(lattice_point) * u;
    double uu = u * u;
    return lattice.get_weight(lattice_point) * dens * (1.0 + cu * lattice.one_over_cs_squared() +
        cu * cu * 0.5 * lattice.one_over_cs_fourth() - 
        uu * 0.5 * lattice.one_over_cs_squared()
    );
}

void LocalCollision::solve() {
    _data->for_each(
        [this](int i, int j, int k){
            _local_collision(i, j, k);
        }
    );
}

void LocalCollision::_local_collision(int i, int j, int q){
    // TODO -> not the most efficient to get density and vel for each lattice point
    auto dens = _data->get_density(i, j);
    auto u = _data->velocity(i, j);
    _data->fstar(q, i, j) = _data->get_f(i, j, q) - _lattice->dt_over_tau() * 
                           (_data->get_f(i, j, q) - single_f_equilibrium(*_lattice, dens, u, q));
}

void PeriodicPropagation::solve() {
    auto periodic = [&](int i, int j, int k){
        const auto nx = _data->dimension(0);
        const auto ny = _data->dimension(1);
        int ni = (i + static_cast<int>(_lattice->get_velocity(k)[0]) + nx) % nx;
        int nj = (j + static_cast<int>(_lattice->get_velocity(k)[1]) + ny) % ny;
        _data->set_f(ni, nj, k, _data->get_fstar(i, j, k));
    };
    _data->for_each(periodic);
}

void MomentUpdateD2Q9::solve() {
    auto d2q9_moments = [&](int i, int j){
        auto& f = _data->f;
        const double* q_block = f.data() + f.index(0, i, j);
        _data->dens(i, j) = std::accumulate(q_block, q_block + _lattice->n_lattice_positions(), 0.0);
        if (_data->dens(i, j) == 0){
            _data->u(i, j).set(0, 0);
        } else {
            double x = (f(1, i, j) + f(5, i, j) + f(8, i, j) - (f(3,i,j) + f(6,i,j) + f(7,i,j))) / _data->dens(i,j); 
            double y = (f(2, i, j) + f(5, i, j) + f(6, i, j) - (f(4,i,j) + f(7,i,j) + f(8,i,j))) / _data->dens(i,j);
            _data->set_velocity(i, j, pba::vec2d(x, y) + _data->get_acceleration(x, y) * _lattice->dt() * 0.5);
        }
    };
    _data->for_each_node(d2q9_moments);
}


void BounceBackHalfwaySolver::solve() {
    const auto& b = get_bounds();
    auto bbhway = [&](int i, int j, int k){
        // if the current point is a boundary, just set f to zero
        const auto nx = _data->dimension(0);
        const auto ny = _data->dimension(1);
        int ni = (i + static_cast<int>(_lattice->get_velocity(k)[0]) + nx) % nx;
        int nj = (j + static_cast<int>(_lattice->get_velocity(k)[1]) + ny) % ny;
        if (b.is_blocked(ni, nj, k)){
            _data->set_f(i, j, _lattice->get_fi_minus(k), _data->get_fstar(i, j, k));
        }
    };
    _data->for_each(bbhway);
}


void LocalCollisionWithForce::solve(){
    auto cf = [&](int i, int j, int k){
        auto fstar = single_f_equilibrium(*_lattice, _data->get_density(i, j), _data->velocity(i, j), k);
        auto collision_op = -1.0 / _lattice->tau() * (_data->get_f(i, j, i) - fstar);
        auto F = _data->get_acceleration(i, j) * _data->dens(i, j);
        auto source = _compute_force_source_term(F, _data->u(i, j), k);
        _data->set_fstar(i, j, k, _data->get_f(i, j, k) + (collision_op + source) * _lattice->dt());
    };
    _data->for_each(cf);
}

double LocalCollisionWithForce::_compute_force_source_term(
    const pba::vec2d& F,
    const pba::vec2d& u,
    int i
) const {
    double s = 0;
    for (int alpha = 0; alpha < 2; alpha++){
        for (int beta = 0; beta < 2; beta++){
            auto t1 = (1.0 - _lattice->dt_over_tau() * 0.5);
            auto t2 = _lattice->get_velocity(i)[alpha] * _lattice->one_over_cs_squared();
            auto t3 = _lattice->get_velocity(i)[alpha] * _lattice->get_velocity(i)[beta];
            t3 = alpha == beta ? t3 - _lattice->c_s() * _lattice->c_s() : t3;
            t3 *= u[beta] * _lattice->one_over_cs_fourth();
            s += t1 * _lattice->get_weight(i) * (t2 - t3) * F[alpha];
        }
    }
    return s;
}


} // end namespace dpa_lbm