#ifndef VOTERMODEL_H
#define VOTERMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlDatabase>
#include "Voter.h"

/**
 * @brief Manages the list of voters (citizens) and their affiliations.
 *
 * @details VoterModel provides an interface to add voters, remove or update them,
 * and query voter data. It uses an SQLite table "voters" and links each voter to a party by ID.
 */
class VoterModel : public QAbstractTableModel {
    Q_OBJECT


signals:
    /** @brief Emitted after a new voter is added to the database. */
    void voterAdded();
    /** @brief Emitted when a voter's details are modified in the database. */
    void voterUpdated();
    /** @brief Emitted after a voter is removed from the database. */
    void voterDeleted();


public:
    /**
     * @brief Constructor for VoterModel.
     * @param connectionName Database connection name (should match PartyModel's connection).
     * @param parent Optional parent object.
     * @param dbPath SQLite database filename (default "politicalsim.sqlite").
     *
     * Opens the SQLite database and ensures the voters table exists (with foreign key link to parties).
     * Loads all existing voter records and prepares the model.
     */
    explicit VoterModel(const QString &connectionName, QObject *parent = nullptr, const QString &dbPath = "politicalsim.sqlite");

    /** @brief Returns the number of voter records in the model. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /** @brief Returns the number of columns in the model (Name, Ideology, Preferred Party). */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /** @brief Retrieves data for a given model index (voter name, ideology, or party name). */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /** @brief Provides header text for each column in the Voter table. */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Adds a new voter to the database.
     * @param voter A Voter struct containing the new voter's details (name, ideology, party).
     *
     * Inserts the voter into the voters table. On success, emits `voterAdded` so the model can refresh.
     */
    void addVoter(const Voter& voter);
    /** @brief Reloads all voter data from the database into the model (e.g., after external changes). */
    void reloadData();
    /**
     * @brief Ensures default voters exist if none are present.
     * @param db An open QSqlDatabase connection.
     * @param partyNameToId Map of party names to their IDs (for assigning party_id to voters).
     * @returns True if the table was empty and default voters were inserted, false if no action needed.
     *
     * Checks if the voters table is empty. If so, populates it with a set of sample voters
     * associated with each default party. Uses the provided map to link voter party names to IDs.
     */
    bool ensureVotersPopulated(QSqlDatabase& db, const QMap<QString, int>& partyNameToId);
    int getVoterIdAt(int row) const;
    void deleteVoterById(int voterId);
    void updateVoter(int id, const Voter &updatedVoter);
    QMap<int, int> countVotersPerParty() const;
    int totalVoters() const;
    Voter getVoterAt(int row) const;


private:
    QString m_connectionName;
    QVector<Voter> m_voters;
};

#endif // VOTERMODEL_H
