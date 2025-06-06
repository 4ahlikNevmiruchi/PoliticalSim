#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "addpartydialog.h"
#include "addvoterdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
        ui->setupUi(this);

    connect(ui->addPartyButton, &QPushButton::clicked, this, [=]() {
        AddPartyDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            Party newParty = dialog.getParty();
            partyModel->addParty(newParty);
        }
    });

    connect(ui->addVoterButton, &QPushButton::clicked, this, [=]() {
        AddVoterDialog dialog(this, partyModel);
        if (dialog.exec() == QDialog::Accepted) {
            Voter newVoter = dialog.getVoter();
            voterModel->addVoter(newVoter);
        }
    });

    connect(ui->deletePartyButton, &QPushButton::clicked, this, [=]() {
        QModelIndex selected = ui->partyTableView->currentIndex();
        if (!selected.isValid()) return;

        int row = selected.row();
        int partyId = partyModel->getPartyIdAt(row);
        partyModel->deletePartyById(partyId);
        //partyModel->reloadData();
        //voterModel->reloadData();
    });

    connect(ui->deleteVoterButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;

        int row = index.row();
        int voterId = voterModel->getVoterIdAt(row);
        if (voterId != -1) {
            voterModel->deleteVoterById(voterId);
        }
        //partyModel->reloadData();
        //voterModel->reloadData();
    });

    connect(ui->editPartyButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->partyTableView->currentIndex();
        int row = index.row();
        if (row < 0) return;

        int id = partyModel->getPartyIdAt(row);
        Party oldParty = partyModel->getPartyAt(row);

        AddPartyDialog dialog(this);
        dialog.setParty(oldParty);

        if (dialog.exec() == QDialog::Accepted) {
            Party updated = dialog.getParty();
            partyModel->updateParty(id, updated);

            //partyModel->reloadData();
            //voterModel->reloadData();
        }
    });

    connect(ui->editVoterButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->voterTableView->currentIndex();
        int row = index.row();
        if (row < 0) return;

        int id = voterModel->getVoterIdAt(row);
        Voter oldVoter = voterModel->getVoterAt(row);

        AddVoterDialog dialog(this, partyModel);
        dialog.setVoter(oldVoter);

        if (dialog.exec() == QDialog::Accepted) {
            Voter updated = dialog.getVoter();
            voterModel->updateVoter(id, updated);
        }
    });

    connect(ui->voterSearchEdit, &QLineEdit::textChanged, this, [=](const QString &text) {
        voterProxyModel->setFilterFixedString(text);
    });

    connect(ui->resetButton, &QPushButton::clicked, this, [=]() {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this,
            "Confirm Reset",
            "Are you sure you want to overwrite all existing data with default values?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply != QMessageBox::Yes) return;

        QSqlDatabase db = QSqlDatabase::database("main_connection");
        if (!db.isOpen()) {
            qWarning() << "Reset failed: DB not open";
            return;
        }

        // Delete all data
        QSqlQuery clear(db);
        if (!clear.exec("DELETE FROM voters")) {
            qWarning() << "[Reset] Failed to clear voters:" << clear.lastError().text();
        }
        if (!clear.exec("DELETE FROM parties")) {
            qWarning() << "[Reset] Failed to clear parties:" << clear.lastError().text();
        }


        // Repopulate
        partyModel->ensurePartiesPopulated(db);
        partyModel->reloadData(); // ensure party table is loaded before mapping

        // Build fresh partyMap
        QMap<QString, int> partyMap;
        for (int i = 0; i < partyModel->rowCount(); ++i) {
            QModelIndex index = partyModel->index(i, 0); // column 0 = name
            QString name = partyModel->data(index, Qt::DisplayRole).toString();
            int id = partyModel->data(index, Qt::UserRole).toInt();
            partyMap.insert(name, id);
        }

        voterModel->ensureVotersPopulated(db, partyMap);
        voterModel->reloadData();
    });

    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel("main_connection", this);
    ui->partyTableView->setModel(partyModel);



    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel->rowCount(); ++i) {
        QModelIndex index = partyModel->index(i, 0);
        QString name = partyModel->data(index, Qt::DisplayRole).toString();
        int id = partyModel->data(index, Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    partyModel->reloadData();

    voterModel = new VoterModel("main_connection", this);
    ui->voterTableView->setModel(voterModel);

    voterModel->reloadData();

    QSqlDatabase db = QSqlDatabase::database("main_connection");
    voterModel->ensureVotersPopulated(db, partyMap);

    voterProxyModel = new QSortFilterProxyModel(this);
    voterProxyModel->setSourceModel(voterModel);
    voterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    voterProxyModel->setFilterKeyColumn(-1); // Search all columns

    ui->voterTableView->setModel(voterProxyModel);


    connect(partyModel, &PartyModel::partyAdded, partyModel, &PartyModel::reloadData);
    connect(partyModel, &PartyModel::partyUpdated, partyModel, &PartyModel::reloadData);
    connect(partyModel, &PartyModel::partyDeleted, partyModel, &PartyModel::reloadData);

    connect(partyModel, &PartyModel::partyUpdated, voterModel, &VoterModel::reloadData);
    connect(partyModel, &PartyModel::partyDeleted, voterModel, &VoterModel::reloadData);

    connect(voterModel, &VoterModel::voterAdded, voterModel, &VoterModel::reloadData);
    connect(voterModel, &VoterModel::voterUpdated, voterModel, &VoterModel::reloadData);
    connect(voterModel, &VoterModel::voterDeleted, voterModel, &VoterModel::reloadData);
}

MainWindow::~MainWindow()
{
    delete ui;
}
