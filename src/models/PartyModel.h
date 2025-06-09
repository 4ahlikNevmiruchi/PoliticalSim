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
    /** @brief Emitted after a new party is successfully added to the database. */
    void partyAdded();
    /** @brief Emitted after a party's details are updated in the database. */
    void partyUpdated();
    /** @brief Emitted after a party is removed from the database. */
    void partyDeleted();
    /** @brief Emitted when external data changes require the party data to refresh (e.g., voter changes). */
    void dataChangedExternally();
    /** @brief Emitted when party popularity percentages are recalculated. */
    void popularityRecalculated();


public:
    /**
     * @brief Constructor for PartyModel.
     * @param connectionName Unique database connection identifier.
     * @param parent Optional parent object.
     * @param seedDefaults Whether to seed default parties if the table is empty.
     * @param dbPath SQLite database filename to use.
     *
     * Opens the SQLite database (creating it if needed), ensures the parties table exists,
     * and optionally seeds it with default parties if empty.
     */
    explicit PartyModel(const QString &connectionName,
                        QObject *parent = nullptr,
                        bool seedDefaults = true,
                        const QString &dbPath = "politicalsim.sqlite");
    /** @brief Returns the number of party records (rows) in the model. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /** @brief Returns the number of columns (fields) in the model (Name, Ideology, Popularity). */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /** @brief Retrieves data for a given model index and role. */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /** @brief Provides header text for each column in the Party table. */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Adds a new party to the database and model.
     * @param party The Party struct containing the party's name and ideology.
     *
     * Inserts a new party into the database and emits a signal to update the model view.
     */
    void addParty(const Party& party);
    /**
     * @brief Ensures default parties exist in the table.
     * @param db An open QSqlDatabase connection for executing queries.
     * @returns True if the table was empty and defaults were inserted, false otherwise.
     *
     * Checks if the parties table has any entries. If not, inserts a predefined set
     * of default political parties (e.g., "Unity Party", "Green Force", etc.).
     */

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


    /**
     * @brief Recalculate party popularity based on current voters and refresh views.
     *
     * This slot is called when the voter data changes. It recomputes the popularity
     * percentages for each party (using the associated VoterModel) and emits signals
     * to update the Party table view and the chart.
     */
public slots:
    void recalculatePopularityFromVoters();


private:
    QVector<Party> m_parties;
    QString m_connectionName;
    QString m_dbPath;
};


#endif // PARTYMODEL_H
