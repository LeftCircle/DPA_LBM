#ifndef _LBMD2Q9_H
#define _LBMD2Q9_H

#include <memory>

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"
#include "lbm_data.h"
#include "algorithm"
#include "numeric"

namespace dpa_lbm{

// Base class for lattice structures.  
// TO DO-> template on vector type
class LBMLattice {
public:
	LBMLattice(double dt, double tau);

	double dt() const noexcept { return _dt; }
	double dt_over_tau() const noexcept { return _dt_over_tau; }
	double one_over_cs_squared() const noexcept { return _one_over_cs_squared; }
	double one_over_cs_fourth() const noexcept { return _one_over_cs_fourth; }
	double tau() const noexcept { return _tau; }
	double c_s() const noexcept { return _c_s; }
	int n_lattice_positions() const noexcept { return _n_lattice_positions; }

	// TODO -> these could be base with each child class
	// instantializing std::vectors
	virtual pba::vec2d get_velocity(int i) const = 0;
	virtual double get_weight(int i) const = 0;

	virtual int get_fi_minus(int q) const = 0; 


	void set_dt(double dt);
	void set_tau(double tau);

protected:
	void _set_speed_of_sound_from_dx_dt();

protected:
	const double _dx = 1.0; // dx in lattice space. fixed to 1 since c_i and dt are based on dx
	double _dt;
	double _c_s; // speed of sound
	double _one_over_cs_squared;
	double _one_over_cs_fourth;
	double _tau; // relaxation time
	double _dt_over_tau;
	int _n_lattice_positions;
};

using LBMLatticePtr = std::shared_ptr<LBMLattice>;

class LBMd2q9 : public LBMLattice{
public:
	static constexpr int N_LATTICE_POSITIONS = 9;
	static constexpr double DEFAULT_w0 = 4.0 / 9.0;
	
public:
	LBMd2q9(double dt, double tau, double w_0 = DEFAULT_w0);

	
	void set_weight0(double w0);
	void set_speed_of_sound(double c_s);

	double get_tau() const { return _tau; }
	double get_speed_of_sound() const { return _c_s; }
	
	pba::vec2d get_velocity(int i) const override { return _ci[i]; }
	double get_weight(int i) const override { return _weights[i]; }

	double advance(LBMData& data, const double t) const;

	void compute_moments(LBMData& data) const;
	
	double single_f_equilibrium(
		const double dens,
		const pba::Vector2<double>& u,
		const int i
	) const;
	void compute_local_collisions(LBMData& data) const;
	void propogate_to_neighbors(LBMData& data) const;
	
	void set_to_equilibrium(LBMData& data) const;

	double compute_max_mach_number(const pba::Vector2<double>& max_vel) const;
	double estimate_reynolds_number(const pba::Vector2<double>& max_vel, float macroscopic_scale) const;
	double get_shear_viscoscity() const;
	double compute_collision_operator(
		LBMData& data,
		int x,
		int y,
		int i
	) const;

	double compute_force_source_term(
		const pba::vec2d& F,
		const pba::vec2d& u,
		int i
	) const;

	// used to calculate f_star with local collision and forces
	double local_collision_and_source(
		LBMData& data,
		int x, 
		int y,
		int q
	) const;

	void propogate_interior_points(LBMData& data) const;
	void propogate_all_points(LBMData& data) const;
	void propogate_periodic_boundary_points(LBMData& data) const;
	void boundary_collision(LBMData& data) const;
	void zero_gradient_outlet(LBMData& data) const;
	void zou_he_velocity_inlet(LBMData& data) const;

	int get_fi_minus(int q) const override { return _fi_minus[q]; };
	
private:
	void _set_distribution_function_dimensions(LBMData& data) const;

private:
	

	double _weights[N_LATTICE_POSITIONS] = {
		4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
	};
	pba::Vector2<double> _ci[N_LATTICE_POSITIONS] = {
		pba::vec2d(0, 0), pba::vec2d(1, 0), pba::vec2d(0, 1),
		pba::vec2d(-1, 0), pba::vec2d(0, -1), pba::vec2d(1, 1),
		pba::vec2d(-1, 1), pba::vec2d(-1, -1), pba::vec2d(1, -1)
	};
	inline static constexpr std::array<int, N_LATTICE_POSITIONS> _fi_minus{
		0, 3, 4, 1, 2, 7, 8, 5, 6
	};
};


} // end namespace dpa_lbm

#endif