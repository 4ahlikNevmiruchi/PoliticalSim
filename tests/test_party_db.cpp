#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>

#include "models/PartyModel.h"

TEST_CASE("PartyModel inserts and loads from persistent DB", "[database]") {
    int argc = 0;
    char *argv[] = { nullptr };
    QCoreApplication app(argc, argv);

    QString connName = "test_connection";
    QString dbFile = "test_parties.sqlite";

    QFile::remove(dbFile);
    QFile::remove("test_parties.sqlite");


    SECTION("Insert party and verify persistence") {
        PartyModel model(connName);
        Party party = { -1, "DB Party", "Techno-Centrism", 42.42 };
        model.addParty(party);

        REQUIRE(model.rowCount() == 1);
        REQUIRE(model.data(model.index(0, 0)).toString() == "DB Party");
        REQUIRE(model.data(model.index(0, 2)).toDouble() == Catch::Approx(42.42));
        qDebug() << "After insert - DB path:" << QSqlDatabase::database(connName).databaseName();
        qDebug() << "DB exists?" << QFile::exists("test_parties.sqlite");

    }

    SECTION("Reload model from same DB file") {
        qDebug() << "Reload section - DB path:" << QSqlDatabase::database(connName).databaseName();
        qDebug() << "DB exists?" << QFile::exists("test_parties.sqlite");

        qDebug() << "[TEST] Reloading from connection:" << connName;

        PartyModel loaded(connName);

        REQUIRE(loaded.rowCount() == 1);
        REQUIRE(loaded.data(loaded.index(0, 0)).toString() == "DB Party");
    }
}
