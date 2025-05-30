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

    ensureVotersPopulated(db);

    query.exec("SELECT name, ideology, party_id FROM voters");
    while (query.next()) {
        Voter v;
        v.name = query.value(0).toString();
        v.ideology = query.value(1).toString();
        v.partyId = query.value(2).isNull() ? -1 : query.value(2).toInt();
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
    if (!index.isValid() || role != Qt::DisplayRole) return {};

    const Voter &voter = m_voters.at(index.row());
    switch (index.column()) {
    case 0: return voter.name;
    case 1: return voter.ideology;
    case 2: return voter.partyId == -1 ? "N/A" : QString::number(voter.partyId);
    }
    return {};
}

QVariant VoterModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};

    switch (section) {
    case 0: return "Name";
    case 1: return "Ideology";
    case 2: return "Preferred Party ID";
    }
    return {};
}

void VoterModel::addVoter(const Voter &voter) {
    beginInsertRows(QModelIndex(), m_voters.size(), m_voters.size());
    m_voters.append(voter);
    endInsertRows();

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
    query.bindValue(":party_id", voter.partyId != -1 ? voter.partyId : QVariant(QVariant::Int));

    if (!query.exec()) {
        qWarning() << "[VoterModel] Insert failed:" << query.lastError().text();
    }
}

bool VoterModel::ensureVotersPopulated(QSqlDatabase& db) {
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM voters")) {
        qWarning() << "[VoterModel] Count query failed:" << countQuery.lastError().text();
        return false;
    }
    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        QSqlQuery insert(db);
        insert.prepare("INSERT INTO voters (name, ideology, party_id) VALUES (?, ?, ?)");
        insert.addBindValue("John Doe");
        insert.addBindValue("Centrist");
        insert.addBindValue(1);  // Matches seeded Unity Party ID
        insert.exec();

        insert.addBindValue("Jane Smith");
        insert.addBindValue("Environmentalism");
        insert.addBindValue(2);  // Matches seeded Green Force ID
        insert.exec();

        qDebug() << "[VoterModel] Seeded default voters.";
        return true;
    }
    return false;
}
