#ifndef VOTERMODEL_H
#define VOTERMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlDatabase>
#include "Voter.h"

class VoterModel : public QAbstractTableModel {
    Q_OBJECT

signals:
    void voterAdded();
    void voterUpdated();
    void voterDeleted();

public:
    explicit VoterModel(const QString &connectionName, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addVoter(const Voter& voter);
    void reloadData();
    bool ensureVotersPopulated(QSqlDatabase& db);
    int getVoterIdAt(int row) const;
    void deleteVoterById(int voterId);
    void updateVoter(int id, const Voter &updatedVoter);

    Voter getVoterAt(int row) const;


private:
    QString m_connectionName;
    QVector<Voter> m_voters;
};

#endif // VOTERMODEL_H
