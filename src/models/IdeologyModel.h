#ifndef IDEOLOGYMODEL_H
#define IDEOLOGYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSqlDatabase>

    struct Ideology {
    int id = -1;
    QString name;
    int centerX = 0;
    int centerY = 0;
};

class IdeologyModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit IdeologyModel(const QString& connectionName, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void seedDefaults(QSqlDatabase& db);

    void loadData();
    int findClosestIdeologyId(int x, int y) const;
    QString getIdeologyNameById(int id) const;
    const QVector<Ideology>& getIdeologies() const;

private:
    QVector<Ideology> m_ideologies;
    QString m_connectionName;
};

#endif // IDEOLOGYMODEL_H
