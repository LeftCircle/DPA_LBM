#ifndef _LBMD2Q9_H
#define _LBMD2Q9_H

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"
#include "lbm_data.h"
#include "algorithm"
#include "numeric"

class LBMd2q9{
public:
	// c_s is speed of sound
	static double advance(LBMData& data, const double t, const double dt);

	static void compute_moments(LBMData& data);
	//static void compute_equilibrium(LBMData& data);
	// c here is the quantized velocity
	static double single_f_equilibrium(
		const double dens,
		const pba::Vector2<double>& u,
		const double one_over_cs_squared,
		const double one_over_cs_fourth,
		const int i
	);
	static void compute_local_collisions(LBMData& data, const double dt);
	static void propogate_to_neighbors(LBMData& data);
	
	static void set_to_equilibrium(LBMData& data, const double dt);


private:
	static void _set_distribution_function_dimensions(LBMData& data);

	inline static const double _weights[9] = {
		4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0,
		1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
	};
	inline static const pba::Vector2<double> _ci[9] = {
		pba::vec2d(0, 0), pba::vec2d(1, 0), pba::vec2d(0, 1),
		pba::vec2d(-1, 0), pba::vec2d(0, -1), pba::vec2d(1, 1),
		pba::vec2d(-1, 1), pba::vec2d(-1, -1), pba::vec2d(1, -1)
	};
};




#endif