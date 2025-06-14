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

#include "utilities/ScopedFileRemover.h"

/*
struct ScopedFileRemover {
    QString path;
    ScopedFileRemover(const QString& filePath) : path(filePath) {}
    ~ScopedFileRemover() {
        if (QFile::exists(path)) {
            QFile::remove(path);
        }
    }
};
*/

QCoreApplication *app = nullptr;

int main(int argc, char* argv[]) {
    QCoreApplication qtApp(argc, argv);
    app = &qtApp;
    return Catch::Session().run(argc, argv);
}

TEST_CASE("PartyModel inserts and loads from persistent DB", "[database]") {
    const QString connName = "test_party_connection";
    const QString dbFile = "test_parties.sqlite";
    //ScopedFileRemover cleanup(dbFile);

    Party testParty;
    testParty.name = "DB Party";
    testParty.ideology = "Techno-Centrism";
    //testParty.popularity = 42.42;

    SECTION("Insert party and verify persistence") {
        PartyModel model(connName, nullptr, false, dbFile);
        model.addParty(testParty);
        model.reloadData();

        REQUIRE(model.rowCount() == 1);
        REQUIRE(model.data(model.index(0, 0)).toString() == "DB Party");
        //REQUIRE(model.data(model.index(0, 2)).toDouble() == Catch::Approx(42.42));
    }

    SECTION("Reload model from same DB file") {
        // Ensure party was already added in the prior section or add here
        PartyModel temp(connName, nullptr, false, dbFile);
        temp.addParty(testParty);

        REQUIRE(QFile::exists(dbFile));

        PartyModel reloaded(connName, nullptr, false, dbFile);
        reloaded.reloadData();

        REQUIRE(reloaded.rowCount() >= 1);
        REQUIRE(reloaded.data(reloaded.index(0, 0)).toString() == "DB Party");
    }
    //QSqlDatabase::removeDatabase(connName);
}
