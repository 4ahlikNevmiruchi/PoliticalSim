#ifndef IDEOLOGYMODEL_H
#define IDEOLOGYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlDatabase>

/**
 * @brief Represents an ideology entry with an ID, name, and center coordinates.
 */
struct Ideology {
    int id = -1;            ///< Unique database ID.
    QString name;           ///< Ideology name.
    int centerX = 0;        ///< Economic axis coordinate.
    int centerY = 0;        ///< Social axis coordinate.
};

/**
 * @brief Table model for ideology definitions from the database.
 *
 * @details This model loads ideology records (ID, name, and center coordinates) from an SQLite database and provides them as a table model for GUI views. It also includes methods to find the nearest ideology to given coordinates and to seed the database with default ideologies if none exist.
 */
class IdeologyModel : public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Constructs the IdeologyModel.
     * @param connectionName The QSQLITE connection name for database access.
     * @param parent Optional parent object.
     */
    explicit IdeologyModel(const QString& connectionName, QObject *parent = nullptr);

    /**
     * @brief Returns the number of ideologies loaded.
     * @param parent Unused.
     * @return Number of rows (ideologies).
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of columns in the model.
     * @param parent Unused.
     * @return Always 3: ID, Name, and Coordinates.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Retrieves data for a given cell and role.
     * @param index Table cell index.
     * @param role Display or other Qt data role.
     * @return The cell's content (ID, name, or formatted coordinates) for DisplayRole; otherwise an empty QVariant.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Provides header labels for the table.
     * @param section Column number (0-based).
     * @param orientation Horizontal or vertical orientation.
     * @param role Data role (only DisplayRole is used).
     * @return Header text ("ID", "Ideology", "Coordinates") for each column.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Seeds the database with default ideologies if none exist.
     * @param db Open QSqlDatabase reference to use for seeding.
     */
    void seedDefaults(QSqlDatabase& db);

    /**
     * @brief Loads all ideologies from the database into the model.
     *
     * Emits the standard model reset signals so any attached views will refresh.
     */
    void loadData();

    /**
     * @brief Finds the ID of the ideology closest to the given coordinates.
     * @param x Economic axis value.
     * @param y Social axis value.
     * @return The ID of the nearest ideology, or -1 if no ideologies are loaded.
     */
    int findClosestIdeologyId(int x, int y) const;

    /**
     * @brief Retrieves the name of an ideology by its ID.
     * @param id The ideology's database ID.
     * @return The ideology name, or an empty string if not found.
     */
    QString getIdeologyNameById(int id) const;

    /**
     * @brief Provides read-only access to the loaded ideologies.
     * @return Const reference to the internal vector of Ideology structs.
     */
    const QVector<Ideology>& getIdeologies() const;

private:
    QVector<Ideology> m_ideologies;             ///< Loaded ideology records.
    QString m_connectionName;                   ///< SQLite connection name.
};

#endif // IDEOLOGYMODEL_H
