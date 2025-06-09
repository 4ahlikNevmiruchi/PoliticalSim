#include <catch2/catch_test_macros.hpp>
#include "models/PartyModel.h"
#include <QFile>

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

TEST_CASE("PartyModel add and get basic info", "[PartyModel]") {
    QString connectionName = "test_connection_party_logic";
    PartyModel model(connectionName, nullptr, false, "test_party_model.sqlite");
    QString dbPath = "test_party_model.sqlite";
    ScopedFileRemover cleanup(dbPath);

    Party p;
    p.name = "Example Party";
    p.ideology = "Moderate";

    model.addParty(p);
    model.reloadData();

    REQUIRE(model.rowCount() >= 1);

    Party retrieved = model.getPartyAt(0);
    REQUIRE(retrieved.name == "Example Party");

    QSqlDatabase::removeDatabase(connectionName);
}
