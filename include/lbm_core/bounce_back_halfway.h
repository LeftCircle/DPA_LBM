#pragma once

#include <cstdint>
#include "nd_vector.h"


// a uint8 because lattice point 0 never moves. 
using d2q9mask = std::uint16_t;


// could be templated for degree, q, mask, ...
class BounceBackHalfway {
public:
    BounceBackHalfway() = default;
    BounceBackHalfway(int x, int y) : _blocked_links(x, y) {}

    void block(int x, int y, int q);
    void block(int x, int y);
    bool is_blocked(int x, int y, int q) const;
    bool is_blocked(int x, int y) const;

private:
    static constexpr d2q9mask _direction_bit(int q) noexcept{
        return d2q9mask{1} << q;
    }

private:
    Array2D<d2q9mask> _blocked_links;
};










