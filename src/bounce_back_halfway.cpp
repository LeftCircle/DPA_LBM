#include "bounce_back_halfway.h"
#include <limits>



bool BounceBackHalfway::is_blocked(int x, int y, int q) const {
    return (_blocked_links(x, y) & _direction_bit(q)) != 0;
}

bool BounceBackHalfway::is_blocked(int x, int y) const {
    return (_blocked_links(x, y) & std::numeric_limits<d2q9mask>::max()) != 0;
}

void BounceBackHalfway::block(int x, int y, int q){
    _blocked_links(x, y) |= _direction_bit(q);
}

void BounceBackHalfway::block(int x, int y){
    _blocked_links(x, y) = std::numeric_limits<d2q9mask>::max();
}




