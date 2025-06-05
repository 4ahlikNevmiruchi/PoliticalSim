#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "addpartydialog.h"
#include "addvoterdialog.h"
#include <QSqlQuery>
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

    connect(ui->resetButton, &QPushButton::clicked, this, [=]() {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this,
            "Confirm Reset",
            "Are you sure you want to overwrite all existing data with the default values?\nThis cannot be undone.",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply != QMessageBox::Yes)
            return;

        QSqlDatabase db = QSqlDatabase::database("main_connection");
        if (!db.isOpen()) {
            qWarning() << "Reset failed: DB not open";
            return;
        }

        QSqlQuery clearQuery(db);
        clearQuery.exec("DELETE FROM voters");
        clearQuery.exec("DELETE FROM parties");

        partyModel->ensurePartiesPopulated(db);
        voterModel->ensureVotersPopulated(db);

        partyModel->reloadData();
        voterModel->reloadData();
    });

    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel("main_connection", this);
    ui->partyTableView->setModel(partyModel);

    partyModel->reloadData();

    voterModel = new VoterModel("main_connection", this);
    ui->voterTableView->setModel(voterModel);

    voterModel->reloadData();

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
