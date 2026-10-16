#ifndef _LBM_DATA_H
#define _LBM_DATA_H

#include <execution>
#include <memory>

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"
#include "bounce_back_halfway.h"

namespace dpa_lbm{

class LBMData{
public:
	
	LBMData(int x_dims, int y_dims, int f_dims);
	

	void set_all_density(double val);
	void set_all_velocity(const pba::Vector2<double>& val);
	void set_all_velocity(double x_vel, double y_vel) { set_all_velocity(pba::Vector2(x_vel, y_vel)); }
	void set_all_f(double val);


	int dimension(int d) const;
	int n_qs() const {return f.get_dim(0); }
	void resize_f_dimension(int d, int to_size) {f.resize_dimension(d, to_size); fstar.resize_dimension(d, to_size); };


	const pba::Vector2<double>& velocity(int x, int y) const { return u(x, y); }
	void set_velocity(int x, int y, const pba::Vector2<double> vel) { u(x, y) = vel; }

	void set_f(int x, int y, int q, double val) { f(q, x, y) = val; }
	const double get_f(int x, int y, int q) const { return f(q, x, y); }

	void set_fstar(int x, int y, int q, double val) { fstar(q, x, y) = val; }
	const double get_fstar(int x, int y, int q) const { return fstar(q, x, y); }

	const auto get_acceleration(int x, int y) const { return _acceleration(x, y); }
	void set_acceleration(int x, int y, pba::vec2d f) { _acceleration(x, y) = f; }

	const auto get_density(int x, int y) const { return dens(x, y); }
	void set_density(int x, int y, double d) { dens(x, y) = d; }

	double get_total_density() const;
	const pba::Vector2<double>& get_max_u() const;

	const BounceBackHalfway& get_bounds() const { return _bounds; }
	BounceBackHalfway& get_bounds() { return _bounds; }

	double get_vorticity(int x, int y) const;
	
	// For each i, j, q, do a function of args func(i, j, q);
	template<typename Func>
	void for_each(Func func){
		const int _y = dimension(1);
    	const int _x = dimension(0);
		const int n_q = n_qs();
		#pragma omp parallel for
		for (int j = 0; j < _y; j++){
        	for (int i = 0; i < _x; i++){
				for (int q = 0; q < n_q; q++){
					func(i, j, q);
				}
			}
		}
	}

	// loops over every i, j, but not the lattice points
	template<typename Func>
	void for_each_node(Func func){
		const int _y = dimension(1);
    	const int _x = dimension(0);
		#pragma omp parallel for
		for (int j = 0; j < _y; j++){
        	for (int i = 0; i < _x; i++){
				func(i, j);
			}
		}
	}

// Currently making data members public although that feels wrong for some reason
public:
	Array2D<double> dens;
	Array2D<pba::Vector2<double>> u;
	
	// f has to be d+1 dimenstions to account for the discretized velocity/distribution function points.
	// Have to be very aware of data access here.
	// Current data access is [q][Nx][Ny] to keep all discretized points together. 
	Array3D<double> f; // distribution function
	// TODO -> consider removing fstar
	Array3D<double> fstar; // intermediate distro function for ease of implementation

private:
	BounceBackHalfway _bounds;
	Array2D<pba::vec2d> _acceleration;

};


using LBMDataPtr = std::shared_ptr<LBMData>;


} // end namespace dpa_lbm

#endif