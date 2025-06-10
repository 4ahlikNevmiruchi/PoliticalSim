#ifndef VOTERMODEL_H
#define VOTERMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlDatabase>
#include "Voter.h"
class PartyModel;
class IdeologyModel;

/**
 * @brief Manages the list of voters (citizens) and their affiliations.
 *
 * @details VoterModel provides an interface to add voters, remove or update them, and query voter data. It uses an SQLite table "voters" and links each voter to a party by ID.
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
     * Opens the SQLite database and ensures the voters table exists (with foreign key link to parties). Loads all existing voter records and prepares the model.
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
     * Checks if the voters table is empty. If so, populates it with a set of sample voters associated with each default party. Uses the provided map to link voter party names to IDs.
     */
    bool ensureVotersPopulated(QSqlDatabase& db, const QMap<QString, int>& partyNameToId);

    /**
     * @brief Updates an existing voter's information.
     * @param id The ID of the voter to update.
     * @param updatedVoter A Voter struct with the new details for the voter.
     *
     * Saves the changes to the database for the given voter ID and updates the model. Emits `voterUpdated` on success.
     */
    void updateVoter(int id, const Voter &updatedVoter);

    /**
     * @brief Removes a voter from the database and model by ID.
     * @param voterId The ID of the voter to remove.
     *
     * Deletes the voter record from the database and updates the model. Emits `voterDeleted` on success.
     */
    void deleteVoterById(int voterId);

    /**
     * @brief Retrieves the Voter at the specified row.
     * @param row The index of the row.
     * @return A copy of the Voter at that row.
     */
    Voter getVoterAt(int row) const;

    /**
     * @brief Returns the ID of the voter at the given row.
     * @param row The row index in the model.
     * @return The voter ID at that row.
     */
    int getVoterIdAt(int row) const;

    /**
     * @brief Returns the total number of voters.
     */
    int totalVoters() const;

    /**
     * @brief Counts how many voters are affiliated with each party.
     * @return A map of party ID to the count of voters in that party.
     */
    QMap<int, int> countVotersPerParty() const;

    /**
     * @brief Finds the ID of the party whose ideology is closest to the given coordinates.
     * @param x The ideology X-coordinate.
     * @param y The ideology Y-coordinate.
     * @return The ID of the nearest party, or -1 if no parties are available.
     */
    int findClosestPartyId(int x, int y) const;

    /**
     * @brief Sets the PartyModel used by this VoterModel.
     * @param model Pointer to the PartyModel providing party data.
     */
    void setPartyModel(const PartyModel* model);

    /**
     * @brief Sets the IdeologyModel used by this VoterModel.
     * @param model Pointer to the IdeologyModel providing ideology data.
     */
    void setIdeologyModel(const IdeologyModel* model);

    /** @brief Recompute each voter’s preferred party. */
    void reassignAllVoterParties();

private:
    QString m_connectionName;               ///< Database connection name.
    QVector<Voter> m_voters;                ///< List of Voter records currently loaded.

    const PartyModel* partyModel = nullptr;             ///< Pointer to the associated PartyModel (for party data).
    const IdeologyModel* ideologyModel = nullptr;       ///< Pointer to the associated IdeologyModel (for ideology data).
};

#endif // VOTERMODEL_H
