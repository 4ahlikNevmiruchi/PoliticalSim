#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <QCoreApplication>
#include "models/PartyModel.h"

using Catch::Approx;

TEST_CASE("PartyModel basic functionality") {
    int argc = 0;
    char *argv[] = { nullptr };
    QCoreApplication app(argc, argv);  // ✅ ✅ ✅

    PartyModel model;
    Party p = { "Test Party", "Test Ideology", 55.0 };
    model.addParty(p);

    QModelIndex index = model.index(0, 0);
    REQUIRE(model.data(index).toString() == "Test Party");

    index = model.index(0, 2);
    REQUIRE(model.data(index).toDouble() == Approx(55.0));
}
