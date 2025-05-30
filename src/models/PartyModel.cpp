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
    db.setDatabaseName("test_parties.sqlite");

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


    query.exec("SELECT name, ideology, popularity FROM parties");
    while (query.next()) {
        m_parties.append(Party{
            query.value(0).toString(),
            query.value(1).toString(),
            query.value(2).toDouble()
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
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const Party& party = m_parties.at(index.row());
    switch (index.column()) {
    case 0: return party.name;
    case 1: return party.ideology;
    case 2: return QString::number(party.popularity, 'f', 2);
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
    beginInsertRows(QModelIndex(), m_parties.size(), m_parties.size());
    m_parties.append(party);
    endInsertRows();

    QSqlDatabase db = QSqlDatabase::database(m_connectionName); //critical
    if (!db.isOpen()) {
        qWarning() << "Add failed: DB not open";
        return;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));

    query.prepare("INSERT INTO parties (name, ideology, popularity) "
                  "VALUES (:name, :ideology, :popularity)");
    query.bindValue(":name", party.name);
    query.bindValue(":ideology", party.ideology);
    query.bindValue(":popularity", party.popularity);

    if (!query.exec()) {
        qWarning() << "Insert failed:" << query.lastError().text();
    }
    qDebug() << "[PartyModel] Connection name: " << m_connectionName;
    qDebug() << "[PartyModel] DB path: " << QSqlDatabase::database(m_connectionName).databaseName();
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
