#include <catch2/catch_test_macros.hpp>
#include "models/PartyModel.h"
//#include "models/VoterModel.h"

TEST_CASE("PartyModel add and get basic info", "[PartyModel]") {
    QString connectionName = "test_connection_party_logic";
    PartyModel model(connectionName, nullptr, false, "test_party_model.sqlite");

    Party p;
    p.name = "Example Party";
    p.ideology = "Moderate";

    model.addParty(p);
    model.reloadData();

    REQUIRE(model.rowCount() >= 1);

    Party retrieved = model.getPartyAt(0);
    REQUIRE(retrieved.name == "Example Party");
}
