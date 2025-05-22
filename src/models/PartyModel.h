#ifndef PARTYMODEL_H
#define PARTYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

struct Party {
    QString name;
    QString ideology;
    double popularity;
};

class PartyModel : public QAbstractTableModel {
    Q_OBJECT

public:
    PartyModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addParty(const Party& party);

private:
    QVector<Party> m_parties;
};

#endif // PARTYMODEL_H