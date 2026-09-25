#include "bounce_back_halfway.h"



BounceBackHalfway::BounceBackHalfway(int x, int y, int q){
    _blocked_links = Array2D<d2q9mask>(x, y);
}


bool BounceBackHalfway::is_blocked(int x, int y, int q) const {
    return _blocked_links(x, y) & _direction_bit(q) != 0;
}






