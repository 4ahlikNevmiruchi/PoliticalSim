#ifndef PARTYMODEL_H
#define PARTYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include <QSqlDatabase>

struct Party {
    int id = -1;
    QString name;
    QString ideology;
    double popularity;
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

public:
    QString m_connectionName;

    explicit PartyModel(const QString &connectionName, QObject *parent = nullptr);

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

    Party getPartyAt(int row) const;


private:
    QVector<Party> m_parties;
};


#endif // PARTYMODEL_H
