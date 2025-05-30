#include "PartyModel.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

//  Custom constructor (used in real app or tests with connection name)
PartyModel::PartyModel(const QString &connectionName, QObject *parent)
    : QAbstractTableModel(parent), m_connectionName(connectionName)
{
    Q_ASSERT(!connectionName.isEmpty());  //Prevent accidental usage

    qDebug() << "[PartyModel] Connection name: " << m_connectionName;


    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
            if (db.isValid()) db.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName("politicalsim.sqlite");

    if (!db.open()) {
        qWarning() << "DB open failed:" << db.lastError();
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS parties ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "ideology TEXT, "
               "popularity REAL)");

    ensurePartiesPopulated(db);

    query.exec("SELECT id, name, ideology, popularity FROM parties");
    while (query.next()) {
        m_parties.append(Party{
            query.value(0).toInt(),         // id
            query.value(1).toString(),      // name
            query.value(2).toString(),      // ideology
            query.value(3).toDouble()       // popularity
        });
    }
    qDebug() << "[PartyModel] Connection name: " << m_connectionName;
    qDebug() << "[PartyModel] DB path: " << QSqlDatabase::database(m_connectionName).databaseName();
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
        case 2: return QString::number(party.popularity, 'f', 2);
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
    case 2: return "Popularity";
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
    query.prepare("INSERT INTO parties (name, ideology, popularity) "
                  "VALUES (:name, :ideology, :popularity)");
    query.bindValue(":name", party.name);
    query.bindValue(":ideology", party.ideology);
    query.bindValue(":popularity", party.popularity);

    if (!query.exec()) {
        qWarning() << "[PartyModel] Insert failed:" << query.lastError().text();
        return;
    }

    // Reload full data from DB
    reloadData();
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
    if (!query.exec("SELECT id, name, ideology, popularity FROM parties")) {
        qWarning() << "[PartyModel] reloadData failed:" << query.lastError().text();
        endResetModel();
        return;
    }

    while (query.next()) {
        m_parties.append(Party{
            query.value(0).toInt(),                     // id
            query.value(1).toString(),                  // name
            query.value(2).toString(),                  // ideology
            query.value(3).toDouble()                   // popularity
        });
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
        insert.exec("INSERT INTO parties (name, ideology, popularity) VALUES ('Unity Party', 'Centrist', 50.0)");
        insert.exec("INSERT INTO parties (name, ideology, popularity) VALUES ('Green Force', 'Environmentalism', 30.0)");
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
    reloadData();
}
