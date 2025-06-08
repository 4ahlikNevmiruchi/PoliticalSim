#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>

#include "models/PartyModel.h"

QCoreApplication *app = nullptr;

/// Catch2 entry point wrapper to create QCoreApplication.
int main(int argc, char* argv[]) {
    QCoreApplication qtApp(argc, argv);
    app = &qtApp;
    return Catch::Session().run(argc, argv);
}

/**
 * @brief Test persistent storage and reloading using PartyModel and SQLite backend.
 */
TEST_CASE("PartyModel inserts and loads from persistent DB", "[database]") {
    const QString connName = "test_party_connection";
    const QString dbFile = "test_parties.sqlite";

    // Full reset
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase::database(connName).close();
        QSqlDatabase::removeDatabase(connName);
    }
    QFile::remove(dbFile);

    SECTION("Insert party and verify persistence") {
        PartyModel model(connName, nullptr, false, dbFile);
        Party testParty;
        testParty.name = "DB Party";
        testParty.ideology = "Techno-Centrism";
        testParty.popularity = 42.42;

        model.addParty(testParty);
        model.reloadData();

        REQUIRE(model.rowCount() == 1);
        REQUIRE(model.data(model.index(0, 0)).toString() == "DB Party");
        REQUIRE(model.data(model.index(0, 2)).toDouble() == Catch::Approx(42.42));
    }

    SECTION("Reload model from same DB file") {
        REQUIRE(QFile::exists(dbFile));

        PartyModel reloaded(connName, nullptr, false, dbFile);
        reloaded.reloadData();

        REQUIRE(reloaded.rowCount() == 1);
        REQUIRE(reloaded.data(reloaded.index(0, 0)).toString() == "DB Party");
    }
}
