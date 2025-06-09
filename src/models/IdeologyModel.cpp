#include "IdeologyModel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

IdeologyModel::IdeologyModel(const QString& connectionName, QObject* parent)
    : QAbstractTableModel(parent), m_connectionName(connectionName)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) {
        qWarning() << "[IdeologyModel] DB not open!";
        return;
    }

    QSqlQuery query(db);
    // 1. Create table if not exists
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS ideologies (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            center_x INTEGER,
            center_y INTEGER
        )
    )")) {
        qWarning() << "[IdeologyModel] Table creation failed:" << query.lastError().text();
    }

    // 2. Seed default data
    seedDefaults(db);

    // 3. Load data into model
    loadData();
}

void IdeologyModel::seedDefaults(QSqlDatabase& db) {
    QSqlQuery count(db);
    if (!count.exec("SELECT COUNT(*) FROM ideologies")) {
        qWarning() << "[IdeologyModel] Count failed:" << count.lastError().text();
        return;
    }
    if (count.next() && count.value(0).toInt() == 0) {
        qDebug() << "[IdeologyModel] Seeding default ideologies";
        QStringList names = {"Centrist", "Environmentalism", "Socialism", "Liberalism", "Conservatism"};
        QVector<QPair<int, int>> coords = {
            {0, 0}, {-50, -50}, {-80, 40}, {60, -30}, {70, 60}
        };

        for (int i = 0; i < names.size(); ++i) {
            QSqlQuery insert(db);
            insert.prepare("INSERT INTO ideologies (name, center_x, center_y) VALUES (:name, :x, :y)");
            insert.bindValue(":name", names[i]);
            insert.bindValue(":x", coords[i].first);
            insert.bindValue(":y", coords[i].second);
            if (!insert.exec()) {
                qWarning() << "[IdeologyModel] Insert failed:" << insert.lastError().text();
            }
        }
    }
}

void IdeologyModel::loadData() {
    beginResetModel();
    m_ideologies.clear();

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.exec("SELECT id, name, center_x, center_y FROM ideologies")) {
        qWarning() << "[IdeologyModel] Load failed:" << query.lastError();
        endResetModel();
        return;
    }

    while (query.next()) {
        m_ideologies.append(Ideology{
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toInt(),
            query.value(3).toInt()
        });
    }

    endResetModel();
}

int IdeologyModel::rowCount(const QModelIndex&) const {
    return m_ideologies.size();
}

int IdeologyModel::columnCount(const QModelIndex&) const {
    return 3; // Name, X, Y
}

QVariant IdeologyModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const Ideology& i = m_ideologies.at(index.row());
    switch (index.column()) {
    case 0: return i.name;
    case 1: return i.centerX;
    case 2: return i.centerY;
    }
    return {};
}

QVariant IdeologyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};

    switch (section) {
    case 0: return "Name";
    case 1: return "X";
    case 2: return "Y";
    }
    return {};
}

int IdeologyModel::findClosestIdeologyId(int x, int y) const {
    int closestId = -1;
    double minDist = std::numeric_limits<double>::max();

    for (const Ideology& i : m_ideologies) {
        double dx = x - i.centerX;
        double dy = y - i.centerY;
        double dist = dx * dx + dy * dy;
        if (dist < minDist) {
            minDist = dist;
            closestId = i.id;
        }
    }

    return closestId;
}

QString IdeologyModel::getIdeologyNameById(int id) const {
    for (const Ideology& i : m_ideologies)
        if (i.id == id) return i.name;
    return "Unknown";
}

const QVector<Ideology>& IdeologyModel::getIdeologies() const {
    return m_ideologies;
}

