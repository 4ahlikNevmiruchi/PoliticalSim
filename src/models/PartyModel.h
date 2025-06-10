#ifndef PARTYMODEL_H
#define PARTYMODEL_H

#include "Voter.h"

#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include <QSqlDatabase>
class VoterModel;
class IdeologyModel;

/**
 * @brief Data structure representing a political party.
 */
struct Party {
    int id = -1;                ///< Unique party ID in the database.
    QString name;               ///< Name of the party.
    int ideologyId = -1;        ///< ID of the associated ideology.
    QString ideology;           ///< Name of the associated ideology.
    int ideologyX = 0;          ///< Economic axis coordinate.
    int ideologyY = 0;          ///< Social axis coordinate.

    /// @brief Equality operator: returns true if two Party instances have the same ID.
    inline bool operator==(const Party& other) const {
        return id == other.id;
    }
};

/**
 * @brief Table model for political parties stored in the database.
 *
 * Provides CRUD operations on the “parties” table, loads and reloads data,
 * and exposes party records to Qt views. Integrates with VoterModel to
 * recalculate popularity and with IdeologyModel to resolve ideology names.
 */
class PartyModel : public QAbstractTableModel {
    Q_OBJECT

signals:
    /** @brief Emitted after a new party is added to the database. */
    void partyAdded();
    /** @brief Emitted after a party's details have been updated in the database. */
    void partyUpdated();
    /** @brief Emitted after a party is removed from the database. */
    void partyDeleted();
    /** @brief Emitted when external changes require the party data to refresh (e.g., a voter modification). */
    void dataChangedExternally();
    /** @brief Emitted when party popularity values are recalculated. */
    void popularityRecalculated();

public:
    /**
     * @brief Constructor for PartyModel.
     * @param connectionName Unique database connection identifier.
     * @param parent Optional parent object.
     * @param seedDefaults Whether to seed default parties if the table is empty.
     * @param dbPath SQLite database filename to use.
     *
     * Opens the SQLite database (creating it if needed), ensures the parties table exists, and optionally seeds it with default parties if empty.
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
     * @brief Updates an existing party's information.
     * @param id The ID of the party to update.
     * @param updatedParty A Party struct with the new details for the party.
     *
     * Applies the changes to the database for the given party ID and refreshes the model. Emits `partyUpdated` on success.
     */
    void updateParty(int id, const Party &updatedParty);

    /**
     * @brief Removes a party from the database and model by its ID.
     * @param partyId The ID of the party to remove.
     *
     * Deletes the party record from the database and updates the model. Emits `partyDeleted` on success.
     */
    void deletePartyById(int partyId);

    /**
     * @brief Ensures default parties exist in the table.
     * @param db An open QSqlDatabase connection for executing queries.
     * @returns True if the table was empty and defaults were inserted, false otherwise.
     *
     * Checks if the parties table has any entries. If not, inserts a predefined set of default political parties (e.g., "Unity Party", "Green Force", etc.).
     */
    bool ensurePartiesPopulated(QSqlDatabase& db);

    /** @brief Reloads all party data from the database into the model (e.g., after external changes). */
    void reloadData();

    /**
     * @brief Provides read-only access to all parties in the model.
     * @return A const reference to the internal list of Party records.
     */
    const QVector<Party>& getAllParties() const;

    /**
     * @brief Retrieves the Party at the specified row.
     * @param row The index of the row.
     * @return A copy of the Party at that row.
     */
    Party getPartyAt(int row) const;

    /**
     * @brief Returns the ID of the party at the given row in the model.
     * @param row The row index in the model.
     * @return The party ID at that row.
     */
    int getPartyIdAt(int row) const;

    /**
     * @brief Calculates the popularity percentage for a given party.
     * @param partyId The party's ID.
     * @return The popularity (fraction between 0 and 1) of the party among all voters.
     */
    double calculatePopularity(const int partyId) const;

    /**
     * @brief Sets the associated VoterModel for this PartyModel.
     * @param model Pointer to the VoterModel providing voter data.
     */
    void setVoterModel(VoterModel* model);

    /**
     * @brief Sets the associated IdeologyModel for this PartyModel.
     * @param model Pointer to the IdeologyModel providing ideology data.
     */
    void setIdeologyModel(const IdeologyModel* model);

public slots:
    /**
     * @brief Recalculate party popularity based on current voters and refresh views.
     *
     * This slot is called when the voter data changes. It recomputes the popularity percentages for each party (using the associated VoterModel) and emits signals to update the Party table view and the chart.
     */
    void recalculatePopularityFromVoters();

private:
    QVector<Party> m_parties;             ///< List of Party records currently loaded.
    QString m_connectionName;             ///< Database connection name.
    QString m_dbPath;                     ///< File path of the SQLite database.

    const IdeologyModel* ideologyModel = nullptr;    ///< Pointer to the IdeologyModel (for ideology data).
    VoterModel* voterModel = nullptr;          ///< Pointer to the VoterModel (for voter data).
};

#endif // PARTYMODEL_H

