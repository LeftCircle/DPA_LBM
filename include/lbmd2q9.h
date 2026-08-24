#ifndef _LBMD2Q9_H
#define _LBMD2Q9_H

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"
#include "lbm_data.h"
#include "algorithm"
#include "numeric"

class LBMd2q9{
public:
	static double advance(LBMData& data, const double t, const double dt);

	static void compute_moments(LBMData& data);
	static void compute_equilibrium(LBMData& data);
	static void compute_local_collisions(LBMData& data);
	static void propogate_to_neighbors(LBMData& data);

private:
	static void _set_distribution_function_dimensions(LBMData& data);

	const double _weights[3][3] = {
		{1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0},
		{1.0 / 9.0, 4.0 / 9.0, 1.0 / 9.0},
		{1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0}
	};
	const pba::Vector2<double> _ci[3][3] = {
		{pba::vec2d(-1, 1), pba::vec2d(0, 1), pba::vec2d(1, 1)},
		{pba::vec2d(-1, 0), pba::vec2d(0, 0), pba::vec2d(1, 0)},
		{pba::vec2d(-1, -1), pba::vec2d(0, -1), pba::vec2d(1, -1)}
	};
};




#endif