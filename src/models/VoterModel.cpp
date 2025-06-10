#include "Voter.h"
#include "VoterModel.h"
#include "PartyModel.h"
#include "IdeologyModel.h"

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
               "ideologyId INTEGER,"
               "ideology_x INTEGER, "
               "ideology_y INTEGER, "
               "party_id INTEGER, "
               "FOREIGN KEY(party_id) REFERENCES parties(id) ON DELETE SET NULL,"
               "FOREIGN KEY(ideologyId) REFERENCES ideologies(id) ON DELETE SET NULL)");

    query.exec(R"(
        SELECT v.id, v.name, i.name, v.ideologyId, v.ideology_x, v.ideology_y, v.party_id, p.name
        FROM voters v
        LEFT JOIN ideologies i ON v.ideologyId = i.id
        LEFT JOIN parties p ON v.party_id = p.id
    )");

    while (query.next()) {
        Voter v;
        v.id = query.value(0).toInt();
        v.name = query.value(1).toString();
        v.ideology = query.value(2).toString();
        v.ideologyId = query.value(3).toInt();
        v.ideologyX = query.value(4).toInt();
        v.ideologyY = query.value(5).toInt();
        v.partyId = query.value(6).isNull() ? -1 : query.value(6).toInt();
        v.partyName = query.value(7).toString();
        m_voters.append(v);
    }
}

int VoterModel::rowCount(const QModelIndex &) const {
    return m_voters.size();
}

int VoterModel::columnCount(const QModelIndex &) const {
    return 3; // Name, Ideology, Party ID
}

QVariant VoterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    const Voter& voter = m_voters.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return voter.name;
        case 1: return voter.ideology;                           // ideology name
        case 2: return voter.partyName.isEmpty() ? "N/A" : voter.partyName;  // party name (or N/A)
        }
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
    query.prepare(R"(
    INSERT INTO voters (name, ideologyId, ideology_x, ideology_y, party_id)
    VALUES (:name, :ideologyId, :ix, :iy, :partyId)
    )");
    query.bindValue(":name", voter.name);
    query.bindValue(":ideologyId", voter.ideologyId);
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
    if (!query.exec(R"(
    SELECT v.id, v.name, i.name AS ideologyName, v.ideologyId, v.ideology_x, v.ideology_y,
           v.party_id, p.name AS partyName
    FROM voters v
    LEFT JOIN ideologies i ON v.ideologyId = i.id
    LEFT JOIN parties p ON v.party_id = p.id
    )"))
     {
        qWarning() << "[VoterModel] reloadData failed:" << query.lastError().text();
        endResetModel();
        return;
    }

    while (query.next()) {
        Voter v;
        v.id = query.value(0).toInt();
        v.name = query.value(1).toString();
        v.ideologyId = query.value(3).toInt();
        v.ideology = query.value(2).toString();
        v.ideologyX = query.value(4).toInt();
        v.ideologyY = query.value(5).toInt();
        v.partyId = query.value(6).isNull() ? -1 : query.value(6).toInt();
        v.partyName = query.value(7).toString();

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
            { -1, "John Doe", "", -1, -1, "", 0, 0 },
            { -1, "Emma Clark", "", -1, -1, "", 2, -2 },
            { -1, "Liam Brooks", "", -1, -1, "", -3, 1 },
            { -1, "Noah Wright", "", -1, -1, "", 5, -3 },
            { -1, "Olivia Lee", "", -1, -1, "", 7, 0 },
            { -1, "James Hall", "", -1, -1, "", 0, 3 },
            { -1, "Sophia Green", "", -1, -1, "", 6, 2 },
            { -1, "Lucas Adams", "", -1, -1, "", -5, -2 },
            { -1, "Mia Evans", "", -1, -1, "", 3, 2 },
            { -1, "Benjamin Scott", "", -1, -1, "", 8, 4 },
            { -1, "Ava Turner", "", -1, -1, "", 0, 0 },
            { -1, "Ethan Lewis", "", -1, -1, "", 9, -2 },
            { -1, "Isabella Young", "", -1, -1, "", -1, 1 },

            // Green Force (~-50, -50)
            { -1, "Lily Cooper", "", -1, -1, "", -48, -52 },
            { -1, "Mason Reed", "", -1, -1, "", -55, -60 },
            { -1, "Ella Walker", "", -1, -1, "", -45, -48 },
            { -1, "Logan Morris", "", -1, -1, "", -60, -55 },
            { -1, "Chloe Hughes", "", -1, -1, "", -52, -45 },
            { -1, "Elijah Rogers", "", -1, -1, "", -49, -47 },
            { -1, "Grace Foster", "", -1, -1, "", -55, -50 },
            { -1, "Henry Bailey", "", -1, -1, "", -46, -53 },

            // Workers Union (~-80, 40)
            { -1, "Amelia Perez", "", -1, -1, "", -82, 42 },
            { -1, "Jack Kelly", "t", -1, -1, "", -85, 38 },
            { -1, "Aria Rivera", "", -1, -1, "", -78, 41 },
            { -1, "Daniel Bennett", "", -1, -1, "", -83, 45 },
            { -1, "Harper Ramirez", "", -1, -1, "", -75, 35 },
            { -1, "Sebastian Bell", "", -1, -1, "", -72, 43 },
            { -1, "Victoria Morgan", "", -1, -1, "", -84, 37 },
            { -1, "Joseph Murphy", "", -1, -1, "", -76, 42 },
            { -1, "Emily Rivera", "", -1, -1, "", -79, 46 },
            { -1, "Ryan Ward", "", -1, -1, "", -81, 39 },

            // Liberty League (~60, -30)
            { -1, "Zoe Jenkins", "", -1, -1, "", 62, -28 },
            { -1, "David Gray", "", -1, -1, "", 58, -30 },
            { -1, "Scarlett Cox", "", -1, -1, "", 70, -45 },
            { -1, "Leo James", "", -1, -1, "", 60, -25 },
            { -1, "Natalie Bailey", "", -1, -1, "", 63, -33 },
            { -1, "Aiden Diaz", "", -1, -1, "", 65, -29 },
            { -1, "Abigail Wood", "", -1, -1, "", 72, -40 },
            { -1, "Samuel Patterson", " ", -1, -1, "", 61, -20 },
            { -1, "Sofia Martinez", "", -1, -1, "", 66, -32 },
            { -1, "Owen Bell", "", -1, -1, "", 64, -28 },
            { -1, "Lucy Peterson", "", -1, -1, "", 69, -35 },

            // Tradition Front (~70, 60)
            { -1, "Michael Butler", "", -1, -1, "", 68, 62 },
            { -1, "Madison Price", "", -1, -1, "", 72, 58 },
            { -1, "Joshua Barnes", "", -1, -1, "", 75, 65 },
            { -1, "Evelyn Sanders", "", -1, -1, "", 70, 60 },
            { -1, "Luke Ross", "", -1, -1, "", 73, 63 },
            { -1, "Hannah Jenkins", "", -1, -1, "", 77, 66 },
            { -1, "Carter Russell", "", -1, -1, "", 70, 59 },
            { -1, "Leah Coleman", "", -1, -1, "", 74, 62 }
        };

        // Assign correct partyId based on ideology
        for (Voter& v : defaults) {
            v.partyId = findClosestPartyId(v.ideologyX, v.ideologyY);
            qDebug() << "Assigning voter to party ID:" << v.partyId;

            v.ideologyId = ideologyModel->findClosestIdeologyId(v.ideologyX, v.ideologyY);
            qDebug() << "Assigning voter to ideology ID:" << v.ideologyId;
        }

        for (const Voter& voter : defaults) {
            QSqlQuery insert(db);
            insert.prepare(R"(
            INSERT INTO voters (name, ideologyId, ideology_x, ideology_y, party_id)
            VALUES (:name, :ideologyId, :x, :y, :party_id)
            )");

            insert.bindValue(":name", voter.name);
            insert.bindValue(":ideologyId", voter.ideologyId);
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
    query.prepare("UPDATE voters SET name = :name, ideologyId = :ideologyId, ideology_x = :ix, ideology_y = :iy, party_id = :party_id WHERE id = :id");
    query.bindValue(":ideologyId", updatedVoter.ideologyId);
    query.bindValue(":ix", updatedVoter.ideologyX);
    query.bindValue(":iy", updatedVoter.ideologyY); // [MODIFIED]
    if (updatedVoter.partyId != -1) {
        query.bindValue(":party_id", updatedVoter.partyId);
    } else {
        query.bindValue(":party_id", QVariant(QVariant::Int)); // NULL if no party
    }
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

void VoterModel::setIdeologyModel(const IdeologyModel* model) {
    ideologyModel = model;
}
