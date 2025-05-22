#include <catch2/catch_test_macros.hpp>
#include "models/Party.h"

TEST_CASE("Party basic functionality", "[party]") {
    Party p("Democratic Union", "Liberalism", 0.51);
    REQUIRE(p.getName() == "Democratic Union");
    REQUIRE(p.getIdeology() == "Liberalism");
    REQUIRE(p.getPopularity() == Approx(0.51));
}