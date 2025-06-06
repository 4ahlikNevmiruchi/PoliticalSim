#include "Voter.h"
#include "VoterModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

VoterModel::VoterModel(const QString &connectionName, QObject *parent)
    : QAbstractTableModel(parent), m_connectionName(connectionName)
{
    Q_ASSERT(!m_connectionName.isEmpty());

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
        if (db.isValid()) db.close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName("politicalsim.sqlite");

    if (!db.open()) {
        qWarning() << "[VoterModel] DB open failed:" << db.lastError();
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS voters ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "ideology TEXT, "
               "party_id INTEGER, "
               "FOREIGN KEY (party_id) REFERENCES parties(id) ON DELETE SET NULL)");

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
    query.prepare("INSERT INTO voters (name, ideology, party_id) "
                  "VALUES (:name, :ideology, :party_id)");
    query.bindValue(":name", voter.name);
    query.bindValue(":ideology", voter.ideology);
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
    if (!query.exec("SELECT voters.id, voters.name, voters.ideology, voters.party_id, parties.name "
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
        v.partyId = query.value(3).isNull() ? -1 : query.value(3).toInt();
        v.partyName = query.value(4).toString();
        m_voters.append(v);
    }

    endResetModel();
    qDebug() << "[VoterModel] reloadData completed. Rows:" << m_voters.size();
}

bool VoterModel::ensureVotersPopulated(QSqlDatabase& db, const QMap<QString, int>& partyNameToId) {
    QSqlQuery query(db);

    QList<Voter> defaults = {
        { -1, "John Doe", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Lisa Feymann", "Rationalist", partyNameToId.value("Green Force", -1) },
        { -1, "Dmitri Ivanov", "Technocrat", partyNameToId.value("Tech Alliance", -1) },
        { -1, "Sarah Blaine", "Conservative", partyNameToId.value("Tradition Front", -1) },
        { -1, "Marco Velasquez", "Progressive", partyNameToId.value("Unity Party", -1) },
        { -1, "Aya Kim", "Eco-Socialist", partyNameToId.value("Green Force", -1) },
        { -1, "Tom Becker", "Libertarian", partyNameToId.value("Freedom First", -1) },
        { -1, "Emily Carter", "Moderate", partyNameToId.value("Unity Party", -1) },
        { -1, "Nina Patel", "Leftist", partyNameToId.value("Green Force", -1) },
        { -1, "Zhang Wei", "Innovator", partyNameToId.value("Tech Alliance", -1) },
        { -1, "George Foster", "Traditionalist", partyNameToId.value("Tradition Front", -1) },
        { -1, "Laura Díaz", "Centrist", partyNameToId.value("Unity Party", -1) },
        { -1, "Ravi Singh", "Environmentalist", partyNameToId.value("Green Force", -1) },
        { -1, "Mia Wong", "Progressive", partyNameToId.value("Freedom First", -1) },
        { -1, "Ahmed Karim", "Futurist", partyNameToId.value("Tech Alliance", -1) }
    };

    for (const auto& voter : defaults) {
        QSqlQuery insert(db);
        insert.prepare("INSERT INTO voters (name, ideology, party_id) "
                       "VALUES (:name, :ideology, :party_id)");
        insert.bindValue(":name", voter.name);
        insert.bindValue(":ideology", voter.ideology);
        insert.bindValue(":party_id", voter.partyId != -1 ? QVariant(voter.partyId) : QVariant(QVariant::Int));

        if (!insert.exec()) {
            qWarning() << "[ensureVotersPopulated] Insert failed:" << insert.lastError();
            return false;
        }
    }

    return true;
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
