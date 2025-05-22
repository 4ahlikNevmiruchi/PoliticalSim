#include "PartyModel.h"

PartyModel::PartyModel(QObject *parent)
    : QAbstractTableModel(parent) {}

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
}