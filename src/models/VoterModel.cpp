#include "Voter.h"
#include "VoterModel.h"
#include "PartyModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

VoterModel::VoterModel(const QString &connectionName, QObject *parent, const QString &dbPath)
    : QAbstractTableModel(parent), m_connectionName(connectionName)
{
    Q_ASSERT(!m_connectionName.isEmpty());

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
        if (db.isValid()) db.close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "[VoterModel] DB open failed:" << db.lastError();
        return;
    }

    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON");

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS voters ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "ideology TEXT, "
               "ideology_x INTEGER, "
               "ideology_y INTEGER, "
               "party_id INTEGER, "
               "FOREIGN KEY(party_id) REFERENCES parties(id) ON DELETE SET NULL)");

    query.exec("SELECT voters.id, voters.name, voters.ideology, voters.party_id, parties.name "
               "FROM voters "
               "LEFT JOIN parties ON voters.party_id = parties.id");

    while (query.next()) {
        Voter v;
        v.id = query.value(0).toInt();
        v.name = query.value(1).toString();
        v.ideology = query.value(2).toString();
        v.partyId = query.value(3).toInt();
        v.partyName = query.value(4).toString();
    }
}

int VoterModel::rowCount(const QModelIndex &) const {
    return m_voters.size();
}

int VoterModel::columnCount(const QModelIndex &) const {
    return 3; // Name, Ideology, Party ID
}

QVariant VoterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};

    const Voter &voter = m_voters.at(index.row());
    switch (index.column()) {
    case 0: return voter.name;
    case 1: return voter.ideology;
    case 2: return voter.partyName.isEmpty() ? "N/A" : voter.partyName; // ✅ Party name display
    }
    return {};
}

QVariant VoterModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};

    switch (section) {
    case 0: return "Name";
    case 1: return "Ideology";
    case 2: return "Preferred Party";
    }
    return {};
}

void VoterModel::addVoter(const Voter &voter) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[VoterModel] Add failed: DB not open";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO voters (name, ideology, ideology_x, ideology_y, party_id) "
                  "VALUES (:name, :ideology, :ix, :iy, :partyId)");
    query.bindValue(":name", voter.name);
    query.bindValue(":ideology", voter.ideology);
    query.bindValue(":ix", voter.ideologyX);
    query.bindValue(":iy", voter.ideologyY);
    query.bindValue(":partyId", voter.partyId);
    if (voter.partyId != -1) {
        query.bindValue(":party_id", voter.partyId);
    } else {
        query.bindValue(":party_id", QVariant(QVariant::Int)); // NULL
    }

    if (!query.exec()) {
        qWarning() << "[VoterModel] Insert failed:" << query.lastError().text();
        return;
    }

    emit voterAdded();
    //reloadData();
}

void VoterModel::reloadData() {
    beginResetModel();
    m_voters.clear();

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.exec("SELECT voters.id, voters.name, voters.ideology, "
                    "voters.ideology_x, voters.ideology_y, "
                    "voters.party_id, parties.name "

                    "FROM voters "
                    "LEFT JOIN parties ON voters.party_id = parties.id")) {
        qWarning() << "[VoterModel] reloadData failed:" << query.lastError().text();
        endResetModel();
        return;
    }

    while (query.next()) {
        Voter v;
        v.id = query.value(0).toInt();
        v.name = query.value(1).toString();
        v.ideology = query.value(2).toString();
        v.ideologyX = query.value(3).toInt();
        v.ideologyY = query.value(4).toInt();
        v.partyId = query.value(5).isNull() ? -1 : query.value(5).toInt();
        v.partyName = query.value(6).toString();

        m_voters.append(v);
    }

    endResetModel();
    qDebug() << "[VoterModel] reloadData completed. Rows:" << m_voters.size();
}

bool VoterModel::ensureVotersPopulated(QSqlDatabase& db, const QMap<QString, int>& partyNameToId) {
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM voters")) {
        qWarning() << "[ensureVotersPopulated] Count query failed:"
                   << countQuery.lastError().text();
        return false;
    }
    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        // Seed default voters only if none exist
        QList<Voter> defaults = {
            // Unity Party (~0, 0)
            { -1, "John Doe", "Centrist", -1, "", 0, 0 },
            { -1, "Emma Clark", "Centrist", -1, "", 2, -2 },
            { -1, "Liam Brooks", "Centrist", -1, "", -3, 1 },
            { -1, "Noah Wright", "Centrist", -1, "", 5, -3 },
            { -1, "Olivia Lee", "Moderate", -1, "", 7, 0 },
            { -1, "James Hall", "Centrist", -1, "", 0, 3 },
            { -1, "Sophia Green", "Moderate", -1, "", 6, 2 },
            { -1, "Lucas Adams", "Centrist", -1, "", -5, -2 },
            { -1, "Mia Evans", "Centrist", -1, "", 3, 2 },
            { -1, "Benjamin Scott", "Moderate", -1, "", 8, 4 },
            { -1, "Ava Turner", "Centrist", -1, "", 0, 0 },
            { -1, "Ethan Lewis", "Moderate", -1, "", 9, -2 },
            { -1, "Isabella Young", "Centrist", -1, "", -1, 1 },

            // Green Force (~-50, -50)
            { -1, "Lily Cooper", "Environmentalism", -1, "", -48, -52 },
            { -1, "Mason Reed", "Green Politics", -1, "", -55, -60 },
            { -1, "Ella Walker", "Environmentalism", -1, "", -45, -48 },
            { -1, "Logan Morris", "Ecologist", -1, "", -60, -55 },
            { -1, "Chloe Hughes", "Sustainability", -1, "", -52, -45 },
            { -1, "Elijah Rogers", "Environmentalism", -1, "", -49, -47 },
            { -1, "Grace Foster", "Green Politics", -1, "", -55, -50 },
            { -1, "Henry Bailey", "Environmentalism", -1, "", -46, -53 },

            // Workers Union (~-80, 40)
            { -1, "Amelia Perez", "Socialist", -1, "", -82, 42 },
            { -1, "Jack Kelly", "Leftist", -1, "", -85, 38 },
            { -1, "Aria Rivera", "Socialist", -1, "", -78, 41 },
            { -1, "Daniel Bennett", "Marxist", -1, "", -83, 45 },
            { -1, "Harper Ramirez", "Social Democrat", -1, "", -75, 35 },
            { -1, "Sebastian Bell", "Progressive", -1, "", -72, 43 },
            { -1, "Victoria Morgan", "Socialist", -1, "", -84, 37 },
            { -1, "Joseph Murphy", "Progressive", -1, "", -76, 42 },
            { -1, "Emily Rivera", "Left-Wing", -1, "", -79, 46 },
            { -1, "Ryan Ward", "Socialist", -1, "", -81, 39 },

            // Liberty League (~60, -30)
            { -1, "Zoe Jenkins", "Liberal", -1, "", 62, -28 },
            { -1, "David Gray", "Liberal", -1, "", 58, -30 },
            { -1, "Scarlett Cox", "Libertarian", -1, "", 70, -45 },
            { -1, "Leo James", "Liberal", -1, "", 60, -25 },
            { -1, "Natalie Bailey", "Progressive", -1, "", 63, -33 },
            { -1, "Aiden Diaz", "Liberal", -1, "", 65, -29 },
            { -1, "Abigail Wood", "Libertarian", -1, "", 72, -40 },
            { -1, "Samuel Patterson", "Social Liberal", -1, "", 61, -20 },
            { -1, "Sofia Martinez", "Liberal", -1, "", 66, -32 },
            { -1, "Owen Bell", "Liberal", -1, "", 64, -28 },
            { -1, "Lucy Peterson", "Libertarian", -1, "", 69, -35 },

            // Tradition Front (~70, 60)
            { -1, "Michael Butler", "Conservative", -1, "", 68, 62 },
            { -1, "Madison Price", "Conservative", -1, "", 72, 58 },
            { -1, "Joshua Barnes", "Traditionalist", -1, "", 75, 65 },
            { -1, "Evelyn Sanders", "Conservative", -1, "", 70, 60 },
            { -1, "Luke Ross", "Right-Wing", -1, "", 73, 63 },
            { -1, "Hannah Jenkins", "Nationalist", -1, "", 77, 66 },
            { -1, "Carter Russell", "Conservative", -1, "", 70, 59 },
            { -1, "Leah Coleman", "Right-Wing", -1, "", 74, 62 }
        };

        // Assign correct partyId based on ideology
        for (Voter& v : defaults) {
            v.partyId = findClosestPartyId(v.ideologyX, v.ideologyY);
            qDebug() << "Assigning voter to party ID:" << v.partyId;
        }

        for (const Voter& voter : defaults) {
            QSqlQuery insert(db);
            insert.prepare("INSERT INTO voters (name, ideology, ideology_x, ideology_y, party_id) "
                           "VALUES (:name, :ideology, :x, :y, :party_id)");
            insert.bindValue(":name", voter.name);
            insert.bindValue(":ideology", voter.ideology);
            insert.bindValue(":x", voter.ideologyX);
            insert.bindValue(":y", voter.ideologyY);
            insert.bindValue(":party_id",
                             voter.partyId != -1 ? QVariant(voter.partyId) : QVariant());  // NULL if -1
            if (!insert.exec()) {
                qWarning() << "[ensureVotersPopulated] Insert failed:"
                           << insert.lastError().text();
                return false;
            }
        }
        qDebug() << "[VoterModel] Seeded default voters.";
        return true;
    }
    return false;
}

int VoterModel::getVoterIdAt(int row) const {
    if (row >= 0 && row < m_voters.size())
        return m_voters[row].id;
    return -1;
}

void VoterModel::deleteVoterById(int voterId) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("DELETE FROM voters WHERE id = :id");
    query.bindValue(":id", voterId);
    if (!query.exec()) {
        qWarning() << "[VoterModel] Delete failed:" << query.lastError().text();
        return;
    }

    emit voterDeleted();
    //reloadData();
}

void VoterModel::updateVoter(int id, const Voter &updatedVoter) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[VoterModel] Update failed: DB not open";
        return;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE voters SET name = :name, ideology = :ideology, party_id = :party_id WHERE id = :id");
    query.bindValue(":name", updatedVoter.name);
    query.bindValue(":ideology", updatedVoter.ideology);
    query.bindValue(":party_id", updatedVoter.partyId);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "[VoterModel] Update failed:" << query.lastError().text();
    }

    emit voterUpdated();
    //reloadData();
}

Voter VoterModel::getVoterAt(int row) const {
    if (row < 0 || row >= m_voters.size()) return {};
    return m_voters[row];
}

QMap<int, int> VoterModel::countVotersPerParty() const {
    QMap<int, int> counts;
    for (const auto& voter : m_voters) {
        counts[voter.partyId]++;
    }
    return counts;
}

int VoterModel::totalVoters() const {
    return m_voters.size();
}

int VoterModel::findClosestPartyId(int x, int y) const {
    if (!partyModel) {
        qWarning() << "[findClosestPartyId] partyModel not set!";
        return -1;
    }

    int closestId = -1;
    double minDistance = std::numeric_limits<double>::max();

    for (const Party& p : partyModel->getAllParties()) {
        double dx = p.ideologyX - x;
        double dy = p.ideologyY - y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance < minDistance) {
            minDistance = distance;
            closestId = p.id;
        }
    }

    return closestId;
}

void VoterModel::setPartyModel(const PartyModel* model) {
    partyModel = model;
}
/*
void VoterModel::recalculateVoterPreferences() {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen() || !partyModel) return;

    for (Voter& voter : m_voters) {
        int closestParty = findClosestPartyId(voter.ideologyX, voter.ideologyY);
        if (closestParty != voter.partyId) {
            updateVoter(voter.id, {voter.id, voter.name, voter.ideology, closestParty, "", voter.ideologyX, voter.ideologyY});
        }
    }
    reloadData();
}
*/
