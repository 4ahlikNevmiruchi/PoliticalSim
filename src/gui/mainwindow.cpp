#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AddPartyDialog.h"

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

    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel("main_connection", this);
    ui->partyTableView->setModel(partyModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
