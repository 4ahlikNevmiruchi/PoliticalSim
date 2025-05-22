#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "models/PartyModel.h"

TEST_CASE("PartyModel inserts and loads from in-memory DB") {
    int argc = 0;
    char *argv[] = { nullptr };
    QCoreApplication app(argc, argv);

    QString connName = "test_connection";
    PartyModel model(connName, nullptr);

    Party testParty = { "DB Party", "Techno-Centrism", 42.42 };
    model.addParty(testParty);

    REQUIRE(model.rowCount() == 1);
    REQUIRE(model.data(model.index(0, 0)).toString() == "DB Party");
    REQUIRE(model.data(model.index(0, 2)).toDouble() == Catch::Approx(42.42));

    PartyModel loaded(connName, nullptr);
    REQUIRE(loaded.rowCount() == 1);
    REQUIRE(loaded.data(loaded.index(0, 0)).toString() == "DB Party");
}
