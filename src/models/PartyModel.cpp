#include "PartyModel.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

//  Default constructor (delegates to memory-based test DB)
PartyModel::PartyModel()
    : PartyModel("", nullptr)
{}

//  Custom constructor (used in real app or tests with connection name)
PartyModel::PartyModel(const QString &connectionName, QObject *parent)
    : QAbstractTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(":memory:"); // Or set file path in GUI context

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

    query.exec("SELECT name, ideology, popularity FROM parties");
    while (query.next()) {
        m_parties.append(Party{
            query.value(0).toString(),
            query.value(1).toString(),
            query.value(2).toDouble()
        });
    }
}

//  Fallback: still allow simple usage with no args
PartyModel::PartyModel(QObject *parent)
    : PartyModel("", parent)
{}

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

    QSqlQuery query;
    query.prepare("INSERT INTO parties (name, ideology, popularity) "
                  "VALUES (:name, :ideology, :popularity)");
    query.bindValue(":name", party.name);
    query.bindValue(":ideology", party.ideology);
    query.bindValue(":popularity", party.popularity);

    if (!query.exec()) {
        qWarning() << "Insert failed:" << query.lastError().text();
    }
}
