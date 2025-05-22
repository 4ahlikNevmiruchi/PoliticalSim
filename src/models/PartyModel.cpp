#include "PartyModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

PartyModel::PartyModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("parties.sqlite");

    if (!db.open()) {
        qWarning() << "Failed to open DB:" << db.lastError().text();
    } else {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS parties ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT,"
                   "ideology TEXT,"
                   "popularity REAL)");

        // Load existing data
        query.exec("SELECT name, ideology, popularity FROM parties");
        while (query.next()) {
            Party p;
            p.name = query.value(0).toString();
            p.ideology = query.value(1).toString();
            p.popularity = query.value(2).toDouble();
            m_parties.append(p);
        }
    }

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

    // Save to DB
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
