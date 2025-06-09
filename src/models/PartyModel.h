#ifndef PARTYMODEL_H
#define PARTYMODEL_H

#include "Voter.h"
#include "VoterModel.h"

#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include <QSqlDatabase>

struct Party {
    int id = -1;
    QString name;
    QString ideology;
    //double popularity;
};
/**
 * @brief Manages a table model of political parties, using an SQLite database.
 * 
 * @details PartyModel handles adding, storing, and retrieving party information
 * including name, ideology, and popularity. It interacts with an SQLite backend
 * for persistence.
 */
class PartyModel : public QAbstractTableModel {
    Q_OBJECT

signals:
    void partyAdded();
    void partyUpdated();
    void partyDeleted();
    void dataChangedExternally();
    void popularityRecalculated();


public:
    explicit PartyModel(const QString &connectionName,
                        QObject *parent = nullptr,
                        bool seedDefaults = true,
                        const QString &dbPath = "politicalsim.sqlite");

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Adds a new party to the model and database.
     * 
     * @param party The Party struct containing name, ideology, and popularity.
     */
    void addParty(const Party& party);
    bool ensurePartiesPopulated(QSqlDatabase& db);
    int getPartyIdAt(int row) const;
    void deletePartyById(int partyId);
    void reloadData();
    void updateParty(int id, const Party &updatedParty);
    const QVector<Party>& getAllParties() const;


    Party getPartyAt(int row) const;

    const VoterModel* voterModel = nullptr;
    void setVoterModel(VoterModel* model);
    double calculatePopularity(int partyId) const;


/*public slots:
    void recalculatePopularityFromVoters(const VoterModel* voterModel);*/


private:
    QVector<Party> m_parties;
    QString m_connectionName;
    QString m_dbPath;
};


#endif // PARTYMODEL_H
