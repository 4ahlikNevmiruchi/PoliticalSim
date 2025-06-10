#include "PartyModel.h"
#include "Voter.h"
#include "VoterModel.h"
#include "IdeologyModel.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

//  Custom constructor
PartyModel::PartyModel(const QString &connectionName, QObject *parent, bool seedDefaults, const QString &dbPath)
    : QAbstractTableModel(parent), m_connectionName(connectionName)
{
    Q_ASSERT(!connectionName.isEmpty());  // Prevent accidental usage

    qDebug() << "[PartyModel] Connection name: " << m_connectionName;

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
        if (db.isValid()) db.close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "DB open failed:" << db.lastError();
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS parties ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "ideology_id INTEGER, "
               "ideology_x INTEGER, "
               "ideology_y INTEGER, "
               "FOREIGN KEY(ideology_id) REFERENCES ideologies(id) ON DELETE SET NULL)");

    if (seedDefaults)
        ensurePartiesPopulated(db);
}

const QVector<Party>& PartyModel::getAllParties() const {
    return m_parties;
}

int PartyModel::rowCount(const QModelIndex &) const {
    return m_parties.size();
}

int PartyModel::columnCount(const QModelIndex &) const {
    return 3; // name, ideology, popularity
}

QVariant PartyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    const Party& party = m_parties.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return party.name;
        case 1: return party.ideology;
        case 2:
                if (voterModel) {
                double pct = calculatePopularity(party.id);
                return QString::number(pct, 'f', 2);
            } else return "0.00";
        }
    } else if (role == Qt::UserRole) {
        return party.id;  // Expose party ID for linking
    }

    return {};
}

QVariant PartyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case 0: return "Name";
    case 1: return "Ideology";
    case 2: return "Popularity %";
    }
    return {};
}

void PartyModel::addParty(const Party &party) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[PartyModel] addParty: DB not open";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) "
                  "VALUES (:name, :ideology_id, :ix, :iy)");
    query.bindValue(":name", party.name);
    query.bindValue(":ideology_id", party.ideologyId);
    query.bindValue(":ix", party.ideologyX);
    query.bindValue(":iy", party.ideologyY);

    if (!query.exec()) {
        qWarning() << "[PartyModel] Insert failed:" << query.lastError().text();
        return;
    }

    emit partyAdded();
    emit dataChangedExternally();
    //reloadData();
}

void PartyModel::reloadData() {
    beginResetModel();
    m_parties.clear();

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[PartyModel] reloadData: DB not open";
        endResetModel();
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT id, name, ideology_id, ideology_x, ideology_y FROM parties")) {
        qWarning() << "[PartyModel] reloadData failed:" << query.lastError().text();
        endResetModel();
        return;
    }
    while (query.next()) {
        Party party;
        party.id = query.value(0).toInt();
        party.name = query.value(1).toString();
        party.ideologyId = query.value(2).toInt();
        party.ideology = (ideologyModel ? ideologyModel->getIdeologyNameById(party.ideologyId) : QString()); //get name from IdeologyModel
        party.ideologyX = query.value(3).toInt();
        party.ideologyY = query.value(4).toInt();
        m_parties.append(party);
    }
    endResetModel();
}

bool PartyModel::ensurePartiesPopulated(QSqlDatabase& db) {
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM parties")) {
        qWarning() << "[PartyModel] Count query failed:" << countQuery.lastError().text();
        return false;
    }

    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        QSqlQuery insert(db);
        QStringList insertStmts = {
            "INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) VALUES ('Unity Party', 1, 0, 0)",
            "INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) VALUES ('Green Force', 2, -50, -50)",
            "INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) VALUES ('Workers Union', 3, -80, 40)",
            "INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) VALUES ('Liberty League', 4, 60, -30)",
            "INSERT INTO parties (name, ideology_id, ideology_x, ideology_y) VALUES ('Tradition Front', 5, 70, 60)"
        };

        for (const QString& stmt : insertStmts) {
            if (!insert.exec(stmt))
                qWarning() << "[PartyModel] Insert failed:" << insert.lastError().text();
        }

        qDebug() << "[PartyModel] Seeded default parties.";
        return true;
    }

    return false;
}

int PartyModel::getPartyIdAt(int row) const {
    if (row < 0 || row >= m_parties.size()) return -1;
    return m_parties[row].id;
}

void PartyModel::deletePartyById(int partyId) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("DELETE FROM parties WHERE id = :id");
    query.bindValue(":id", partyId);
    if (!query.exec()) {
        qWarning() << "[PartyModel] Delete failed:" << query.lastError().text();
    }

    emit partyDeleted();
    emit dataChangedExternally();
    //reloadData();
}

void PartyModel::updateParty(int id, const Party &updatedParty) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[PartyModel] Update failed: DB not open";
        return;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE parties SET name = :name, ideology_id = :ideology_id, "
                  "ideology_x = :ix, ideology_y = :iy WHERE id = :id");
    query.bindValue(":name", updatedParty.name);
    query.bindValue(":ideology_id", updatedParty.ideologyId);
    query.bindValue(":ix", updatedParty.ideologyX);
    query.bindValue(":iy", updatedParty.ideologyY);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "[PartyModel] Update failed:" << query.lastError().text();
    }

    emit partyUpdated();
    emit dataChangedExternally();
    //reloadData();
}

Party PartyModel::getPartyAt(int row) const {
    if (row < 0 || row >= m_parties.size()) return {};
    return m_parties[row];
}

double PartyModel::calculatePopularity(int partyId) const {
    if (!voterModel) return 0.0;
    int total = voterModel->totalVoters();
    if (total == 0) return 0.0;

    QMap<int, int> map = voterModel->countVotersPerParty();
    return (map.value(partyId, 0) * 100.0) / total;
}

void PartyModel::setVoterModel(VoterModel* model) {
    voterModel = model;
    connect(model, &VoterModel::voterAdded,    this, &PartyModel::recalculatePopularityFromVoters);
    connect(model, &VoterModel::voterUpdated,  this, &PartyModel::recalculatePopularityFromVoters);
    connect(model, &VoterModel::voterDeleted,  this, &PartyModel::recalculatePopularityFromVoters);
}

void PartyModel::recalculatePopularityFromVoters() {
    if (!voterModel) return;
    if (m_parties.isEmpty()) return;
    // Notify that all parties' popularity data has changed (column 2 in the model)
    emit dataChanged(index(0, 2), index(rowCount() - 1, 2));
    // Also notify external listeners (e.g., the chart widget) of data change
    emit dataChangedExternally();
}

void PartyModel::setIdeologyModel(const IdeologyModel* model) {
    ideologyModel = model;
}

