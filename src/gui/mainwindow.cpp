#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "addpartydialog.h"
#include "addvoterdialog.h"

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
        partyModel->reloadData();
        voterModel->reloadData();
    });

    connect(ui->deleteVoterButton, &QPushButton::clicked, this, [=]() {
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;

        int row = index.row();
        int voterId = voterModel->getVoterIdAt(row);
        if (voterId != -1) {
            voterModel->deleteVoterById(voterId);
        }
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
}

MainWindow::~MainWindow()
{
    delete ui;
}
