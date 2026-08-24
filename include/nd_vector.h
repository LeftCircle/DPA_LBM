#ifndef _ND_VECTOR_H
#define _ND_VECTOR_H

#include <vector>
#include <algorithm>
#include <array>

template <typename T>
class Array2D {
public:
	using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

	Array2D(int x_dim, int y_dim) : _dims{x_dim, y_dim} {
		_data.resize(x_dim * y_dim);
	}

	int index(int x_idx, int y_idx) const noexcept {
		return x_idx + _dims[0] * y_idx;
	}

	T& operator()(int x_idx, int y_idx) {
		return _data[index(x_idx, y_idx)];
	}

	const T& operator()(int x_idx, int y_idx) const {
		return _data[index(x_idx, y_idx)];
	}

	int get_x_dim() const noexcept { return _dims[0]; }
	int get_y_dim() const noexcept { return _dims[1]; }
	int get_dim(int d) const { return _dims[d]; }

	void clear() {
		std::fill(_data.begin(), _data.end(), T());
	}

	void fill(T val){
		std::fill(_data.begin(), _data.end(), val);
	}

	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end() }

	const_iterator begin() const { return _data.begin(); }
	const_iterator end() const { return _data.end(); }

private:
	int _dims[2];
	std::vector<T> _data;
};

template <typename T>
class Array3D {
public:
	Array3D(int x_dim, int y_dim, int z_dim) : _dims{x_dim, y_dim, z_dim} {
		_data.resize(x_dim * y_dim * z_dim);
	}

	int index(int x_idx, int y_idx, int z_idx) const noexcept {
		return x_idx + _dims[0] * (y_idx + _dims[1] * z_idx);
	}

	T& operator()(int x_idx, int y_idx, int z_idx) {
		return _data[index(x_idx, y_idx, z_idx)];
	}

	const T& operator()(int x_idx, int y_idx, int z_idx) const {
		return _data[index(x_idx, y_idx, z_idx)];
	}

	int get_x_dim() const noexcept { return _dims[0]; }
	int get_y_dim() const noexcept { return _dims[1]; }
	int get_z_dim() const noexcept { return _dims[2]; }
	int get_dim(int d) const { return _dims[d]; }

	void resize_dimension(int d, int size) {
		_dims[d] = size;
		_data.resize(_dims[0] * _dims[1] * _dims[2]);
	}

	void clear() {
		std::fill(_data.begin(), _data.end(), T());
	}

	void fill(T val){
		std::fill(_data.begin(), _data.end(), val);
	}

private:
	int _dims[3];
	std::vector<T> _data;
};


#endif



/*
The claude version of templating this to take any dimension. 


#ifndef _ND_VECTOR_H
#define _ND_VECTOR_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

template <typename T, std::size_t N>
class NDArray {
	static_assert(N >= 1, "NDArray requires at least one dimension");

public:
	// Constructor: exactly N integral sizes, e.g. NDArray<float,3>(w,h,d)
	template <typename... Dims,
			typename = std::enable_if_t<sizeof...(Dims) == N>>
	explicit NDArray(Dims... dims) : _dims{static_cast<int>(dims)...} {
		_data.resize((static_cast<std::size_t>(dims) * ...));
	}

	// Index computation: idx0 + dim0*(idx1 + dim1*(idx2 + ...))
	template <typename... Idx>
	int index(Idx... idx) const noexcept {
		static_assert(sizeof...(Idx) == N, "wrong number of indices");
		std::array<int, N> i{static_cast<int>(idx)...};
		int result = 0;
		int stride = 1;
		for (std::size_t d = 0; d < N; ++d) {
			result += i[d] * stride;
			stride *= _dims[d];
		}
		return result;
	}

	template <typename... Idx>
	T& operator()(Idx... idx) {
		return _data[index(idx...)];
	}

	template <typename... Idx>
	const T& operator()(Idx... idx) const {
		return _data[index(idx...)];
	}

	int dim(std::size_t d) const noexcept { return _dims[d]; }

	void clear() { std::fill(_data.begin(), _data.end(), T()); }

private:
	std::array<int, N> _dims;
	std::vector<T> _data;
};

// Keep your original names/API shape as thin aliases
template <typename T> using Array2D = NDArray<T, 2>;
template <typename T> using Array3D = NDArray<T, 3>;

#endif

*/