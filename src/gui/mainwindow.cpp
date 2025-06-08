#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "addpartydialog.h"
#include "addvoterdialog.h"
#include "widgets/PartyChartWidget.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

PartyChartWidget *partyChart;

/**
 * @brief MainWindow::MainWindow Constructor for main UI window
 *
 * - Initializes models (Party/Voter)
 * - Binds models to views and filters
 * - Populates default data if DB empty
 * - Sets up chart widget inside `chartContainer`
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel("main_connection", this);
    voterModel = new VoterModel("main_connection", this);
    voterProxyModel = new QSortFilterProxyModel(this);

    // Models → Views
    ui->partyTableView->setModel(partyModel);
    voterProxyModel->setSourceModel(voterModel);
    voterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    voterProxyModel->setFilterKeyColumn(-1);
    ui->voterTableView->setModel(voterProxyModel);

    // Database
    QSqlDatabase db = QSqlDatabase::database("main_connection");
    partyModel->ensurePartiesPopulated(db);
    partyModel->reloadData();

    // Create map of name → id for assigning to voters
    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel->rowCount(); ++i) {
        QModelIndex idx = partyModel->index(i, 0);
        QString name = partyModel->data(idx, Qt::DisplayRole).toString();
        int id = partyModel->data(idx, Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    voterModel->ensureVotersPopulated(db, partyMap);
    voterModel->reloadData();

    // Connect search bar to proxy
    connect(ui->voterSearchEdit, &QLineEdit::textChanged, this, [=](const QString &text) {
        voterProxyModel->setFilterFixedString(text);
    });

    // 🔄 Real-Time Chart Setup
    partyChart = new PartyChartWidget(partyModel, this);
    if (!ui->chartContainer->layout()) {
        auto *layout = new QVBoxLayout(ui->chartContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->chartContainer->setLayout(layout);
    }
    ui->chartContainer->layout()->addWidget(partyChart);
    partyChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Optional: Remove minSize if it causes clipping
    ui->chartContainer->setMinimumSize(QSize(0, 0));

    setupButtonConnections();
}

void MainWindow::setupButtonConnections() {
    connect(ui->addPartyButton, &QPushButton::clicked, this, [=]() {
        AddPartyDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            partyModel->addParty(dialog.getParty());
        }
    });

    connect(ui->editPartyButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->partyTableView->currentIndex();
        if (!index.isValid()) return;
        int id = partyModel->getPartyIdAt(index.row());
        Party party = partyModel->getPartyAt(index.row());

        AddPartyDialog dialog(this);
        dialog.setParty(party);
        if (dialog.exec() == QDialog::Accepted) {
            partyModel->updateParty(id, dialog.getParty());
        }
    });

    connect(ui->deletePartyButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->partyTableView->currentIndex();
        if (!index.isValid()) return;
        int id = partyModel->getPartyIdAt(index.row());
        partyModel->deletePartyById(id);
    });

    connect(ui->addVoterButton, &QPushButton::clicked, this, [=]() {
        AddVoterDialog dialog(this, partyModel);
        if (dialog.exec() == QDialog::Accepted) {
            voterModel->addVoter(dialog.getVoter());
        }
    });

    connect(ui->editVoterButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;
        int id = voterModel->getVoterIdAt(index.row());
        Voter voter = voterModel->getVoterAt(index.row());

        AddVoterDialog dialog(this, partyModel);
        dialog.setVoter(voter);
        if (dialog.exec() == QDialog::Accepted) {
            voterModel->updateVoter(id, dialog.getVoter());
        }
    });

    connect(ui->deleteVoterButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;
        int id = voterModel->getVoterIdAt(index.row());
        voterModel->deleteVoterById(id);
    });

    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::resetDatabase);
}

void MainWindow::resetDatabase() {
    auto reply = QMessageBox::warning(
        this, "Confirm Reset",
        "Are you sure you want to overwrite all existing data with default values?",
        QMessageBox::Yes | QMessageBox::No
        );
    if (reply != QMessageBox::Yes) return;

    QSqlDatabase db = QSqlDatabase::database("main_connection");
    if (!db.isOpen()) {
        qWarning() << "Reset failed: DB not open";
        return;
    }

    QSqlQuery clear(db);
    if (!clear.exec("DELETE FROM voters"))
        qWarning() << "[Reset] Failed to clear voters:" << clear.lastError().text();
    if (!clear.exec("DELETE FROM parties"))
        qWarning() << "[Reset] Failed to clear parties:" << clear.lastError().text();

    partyModel->ensurePartiesPopulated(db);
    partyModel->reloadData();

    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel->rowCount(); ++i) {
        QModelIndex index = partyModel->index(i, 0);
        QString name = partyModel->data(index, Qt::DisplayRole).toString();
        int id = partyModel->data(index, Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    voterModel->ensureVotersPopulated(db, partyMap);
    voterModel->reloadData();
}

MainWindow::~MainWindow()
{
    delete ui;
}
