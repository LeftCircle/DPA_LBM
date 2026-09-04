#ifndef _LBMD2Q9_H
#define _LBMD2Q9_H

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"
#include "lbm_data.h"
#include "algorithm"
#include "numeric"

const double default_w0 = 4.0 / 9.0;

class LBMd2q9{
public:
	LBMd2q9(double dt, double tau, double w_0 = default_w0);

	void set_dt(double dt);
	void set_tau(double tau);
	void set_weight0(double w0);

	// c_s is speed of sound
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

private:
	
	void _set_speed_of_sound_from_dx_dt();
	void _set_distribution_function_dimensions(LBMData& data) const;
	void _set_local_velocities_based_on_dt_and_weights();

	static const int _n_lattice_positions = 9;
	const double _dx = 1.0; // dx in lattice space. fixed to 1 since c_i and dt are based on dx
	double _dt;
	double _c_s; // speed of sound
	double _one_over_cs_squared;
	double _one_over_cs_fourth;
	double _tau; // relaxation time
	double _dt_over_tau;

	double _weights[_n_lattice_positions] = {
		4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
	};
	pba::Vector2<double> _ci[_n_lattice_positions] = {
		pba::vec2d(0, 0), pba::vec2d(1, 0), pba::vec2d(0, 1),
		pba::vec2d(-1, 0), pba::vec2d(0, -1), pba::vec2d(1, 1),
		pba::vec2d(-1, 1), pba::vec2d(-1, -1), pba::vec2d(1, -1)
	};
};




#endif