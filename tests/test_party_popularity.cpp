#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QFile>
#include <QMap>

#include "models/PartyModel.h"
#include "models/VoterModel.h"

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

TEST_CASE("Party popularity is derived from voters", "[derived-popularity]") {
    const QString connName = "test_popularity_connection";
    const QString dbPath = "test_popularity.sqlite";
    ScopedFileRemover cleanup(dbPath);

    // Init models
    PartyModel partyModel(connName, nullptr, false, dbPath);
    VoterModel voterModel(connName, nullptr, dbPath);
    partyModel.setVoterModel(&voterModel);

    // Manual DB ref
    QSqlDatabase db = QSqlDatabase::database(connName);

    // Add two parties
    Party p1 { -1, "Alpha", "Lib" };
    Party p2 { -1, "Beta", "Con" };
    partyModel.addParty(p1);
    partyModel.addParty(p2);
    partyModel.reloadData();

    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel.rowCount(); ++i) {
        QString name = partyModel.data(partyModel.index(i, 0)).toString();
        int id = partyModel.data(partyModel.index(i, 0), Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    // Seed 3 Alpha votes, 1 Beta vote
    QList<Voter> voters = {
                           { -1, "A", "X", partyMap["Alpha"] },
                           { -1, "B", "X", partyMap["Alpha"] },
                           { -1, "C", "X", partyMap["Alpha"] },
                           { -1, "D", "X", partyMap["Beta"] },
                           };

    for (const auto& v : voters)
        voterModel.addVoter(v);

    voterModel.reloadData();
    partyModel.reloadData();

    double alphaPopularity = partyModel.calculatePopularity(1);
    double betaPopularity = partyModel.calculatePopularity(2);

    REQUIRE(alphaPopularity == Catch::Approx(75.0));
    REQUIRE(betaPopularity == Catch::Approx(25.0));

    double alphaPopularity2 = partyModel.data(partyModel.index(0, 2)).toDouble();
    double betaPopularity2 = partyModel.data(partyModel.index(1, 2)).toDouble();

    REQUIRE(alphaPopularity2 == Catch::Approx(75.0));
    REQUIRE(betaPopularity2 == Catch::Approx(25.0));

    QSqlDatabase::removeDatabase(connName);
}
