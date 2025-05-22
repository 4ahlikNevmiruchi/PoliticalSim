#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel(this);
    ui->partyTableView->setModel(partyModel);

    // Optional: populate with test data
    partyModel->addParty({"Unity Party", "Centrist", 35.5});
    partyModel->addParty({"Green Force", "Environmentalism", 15.2});
}

MainWindow::~MainWindow()
{
    delete ui;
}
