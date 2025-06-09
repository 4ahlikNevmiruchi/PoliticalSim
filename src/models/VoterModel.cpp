#include "Voter.h"
#include "VoterModel.h"
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
        // Unity Party (13)
        { -1, "John Doe", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Emma Clark", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Liam Brooks", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Noah Wright", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Olivia Lee", "Moderate", partyNameToId.value("Unity Party", -1) },
        { -1, "James Hall", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Sophia Green", "Moderate", partyNameToId.value("Unity Party", -1) },
        { -1, "Lucas Adams", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Mia Evans", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Benjamin Scott", "Moderate", partyNameToId.value("Unity Party", -1) },
        { -1, "Ava Turner", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Ethan Lewis", "Moderate", partyNameToId.value("Unity Party", -1) },
        { -1, "Isabella Young", "Centrist", partyNameToId.value("Unity Party", -1) },

        // Green Force (8)
        { -1, "Lily Cooper", "Environmentalism", partyNameToId.value("Green Force", -1) },
        { -1, "Mason Reed", "Green Politics", partyNameToId.value("Green Force", -1) },
        { -1, "Ella Walker", "Environmentalism", partyNameToId.value("Green Force", -1) },
        { -1, "Logan Morris", "Ecologist", partyNameToId.value("Green Force", -1) },
        { -1, "Chloe Hughes", "Sustainability", partyNameToId.value("Green Force", -1) },
        { -1, "Elijah Rogers", "Environmentalism", partyNameToId.value("Green Force", -1) },
        { -1, "Grace Foster", "Green Politics", partyNameToId.value("Green Force", -1) },
        { -1, "Henry Bailey", "Environmentalism", partyNameToId.value("Green Force", -1) },

        // Workers Union (10)
        { -1, "Amelia Perez", "Socialist", partyNameToId.value("Workers Union", -1) },
        { -1, "Jack Kelly", "Leftist", partyNameToId.value("Workers Union", -1) },
        { -1, "Aria Rivera", "Socialist", partyNameToId.value("Workers Union", -1) },
        { -1, "Daniel Bennett", "Marxist", partyNameToId.value("Workers Union", -1) },
        { -1, "Harper Ramirez", "Social Democrat", partyNameToId.value("Workers Union", -1) },
        { -1, "Sebastian Bell", "Progressive", partyNameToId.value("Workers Union", -1) },
        { -1, "Victoria Morgan", "Socialist", partyNameToId.value("Workers Union", -1) },
        { -1, "Joseph Murphy", "Progressive", partyNameToId.value("Workers Union", -1) },
        { -1, "Emily Rivera", "Left-Wing", partyNameToId.value("Workers Union", -1) },
        { -1, "Ryan Ward", "Socialist", partyNameToId.value("Workers Union", -1) },

        // Liberty League (11)
        { -1, "Zoe Jenkins", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "David Gray", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Scarlett Cox", "Libertarian", partyNameToId.value("Liberty League", -1) },
        { -1, "Leo James", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Natalie Bailey", "Progressive", partyNameToId.value("Liberty League", -1) },
        { -1, "Aiden Diaz", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Abigail Wood", "Libertarian", partyNameToId.value("Liberty League", -1) },
        { -1, "Samuel Patterson", "Social Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Sofia Martinez", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Owen Bell", "Liberal", partyNameToId.value("Liberty League", -1) },
        { -1, "Lucy Peterson", "Libertarian", partyNameToId.value("Liberty League", -1) },

        // Tradition Front (8)
        { -1, "Michael Butler", "Conservative", partyNameToId.value("Tradition Front", -1) },
        { -1, "Madison Price", "Conservative", partyNameToId.value("Tradition Front", -1) },
        { -1, "Joshua Barnes", "Traditionalist", partyNameToId.value("Tradition Front", -1) },
        { -1, "Evelyn Sanders", "Conservative", partyNameToId.value("Tradition Front", -1) },
        { -1, "Luke Ross", "Right-Wing", partyNameToId.value("Tradition Front", -1) },
        { -1, "Hannah Jenkins", "Nationalist", partyNameToId.value("Tradition Front", -1) },
        { -1, "Carter Russell", "Conservative", partyNameToId.value("Tradition Front", -1) },
        { -1, "Leah Coleman", "Right-Wing", partyNameToId.value("Tradition Front", -1) }
        };
        for (const Voter& voter : defaults) {
            QSqlQuery insert(db);
            insert.prepare("INSERT INTO voters (name, ideology, party_id) "
                           "VALUES (:name, :ideology, :party_id)");
            insert.bindValue(":name", voter.name);
            insert.bindValue(":ideology", voter.ideology);
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
