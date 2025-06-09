#include <catch2/catch_test_macros.hpp>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>

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

TEST_CASE("VoterModel basic add/load", "[voter]") {
    const QString connName = "test_voter_connection";
    const QString dbPath = "test_voter.sqlite";
    //ScopedFileRemover cleanup(dbPath);

    qDebug() << "[TEST] Start test for connection:" << connName;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            qFatal("[TEST] Cannot open database during setup");
        }

        {
            QSqlQuery query(db);
            query.exec("CREATE TABLE IF NOT EXISTS parties (id INTEGER PRIMARY KEY, name TEXT, ideology TEXT, popularity REAL);");
            query.exec("CREATE TABLE IF NOT EXISTS voters (id INTEGER PRIMARY KEY, name TEXT, ideology TEXT, party_id INTEGER);");
            query.exec("INSERT INTO parties (id, name, ideology, popularity) VALUES (1, 'Unity Party', 'Centrist', 50.0);");
        }

        db.close();
    }

    QMap<QString, int> nameToId;
    nameToId["Unity Party"] = 1;

    {
        qDebug() << "[TEST] Creating VoterModel with custom DB";
        auto* model = new VoterModel(connName, nullptr, dbPath);

        {
            QSqlDatabase db = QSqlDatabase::database(connName);
            if (!db.open()) {
                qFatal("[TEST] Failed to open database inside model");
            }

            qDebug() << "[TEST] Seeding voters";
            REQUIRE(model->ensureVotersPopulated(db, nameToId) == true);
        }

        //delete model;
    }
    //QSqlDatabase::removeDatabase(connName);
}
