#pragma once

#include <cstdint>
#include "nd_vector.h"


// a uint8 because lattice point 0 never moves. 
using d2q9mask = std::uint8_t;


// could be templated for degree, q, mask, ...
class BounceBackHalfway {
public:
    BounceBackHalfway(int x, int y, int q);

    bool is_blocked(int x, int y, int q) const;

private:
    static constexpr d2q9mask _direction_bit(int q) noexcept{
        return (q >= 1 && q <= 8)
            ? d2q9mask{1} << (q - 1)
            : d2q9mask{0};
        }

private:
    Array2D<d2q9mask> _blocked_links;
};










