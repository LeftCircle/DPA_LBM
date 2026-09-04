#ifndef _LBM_DATA_H
#define _LBM_DATA_H

#include "tessendorf_vecendorf.h"
#include "nd_vector.h"

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
};



#endif