#include <catch2/catch_test_macros.hpp>
#include "models/PartyModel.h"

TEST_CASE("PartyModel add and get", "[PartyModel]") {
    QString connectionName = "test_connection_party";
    PartyModel model(connectionName);

    Party p;
    p.name = "Example Party";
    p.ideology = "Moderate";
    p.popularity = 20.0;

    model.addParty(p);
    model.reloadData();

    REQUIRE(model.rowCount() > 0);

    Party retrieved = model.getPartyAt(0);
    REQUIRE(retrieved.name == "Example Party");
}
