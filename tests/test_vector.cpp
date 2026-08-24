#include "catch_helpers.h"


using namespace pba;


TEST_CASE("Test vec2 add"){
    Vector2<int> a(1, 2);
    Vector2<int> b(3, 4);

    Vector2 c = a + b;
    REQUIRE(c.X() == 4);
    REQUIRE(c.Y() == 6);
}


