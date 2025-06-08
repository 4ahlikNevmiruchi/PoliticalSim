#include <catch2/catch_test_macros.hpp>
#include "models/VoterModel.h"
#include "models/Voter.h"
#include <QSqlDatabase>
#include <QSqlQuery>

TEST_CASE("VoterModel basic add/load", "[VoterModel]") {
    QString connectionName = "test_connection_voters";
    VoterModel model(connectionName);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QMap<QString, int> mockPartyMap;
    mockPartyMap["Test Party"] = 1;

    REQUIRE(model.ensureVotersPopulated(db, mockPartyMap) == true);
    model.reloadData();

    REQUIRE(model.rowCount() > 0);

    Voter v = model.getVoterAt(0);
    REQUIRE(!v.name.isEmpty());
}
