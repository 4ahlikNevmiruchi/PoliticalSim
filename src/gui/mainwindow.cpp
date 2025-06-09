#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "addpartydialog.h"
#include "addvoterdialog.h"
#include "models/PartyModel.h"
#include "models/IdeologyModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("PoliticalSim");

    partyModel = new PartyModel("main_connection", this);
    voterModel = new VoterModel("main_connection", this);
    ideologyModel = new IdeologyModel("main_connection", this);

    partyModel->setVoterModel(voterModel);
    voterModel->setPartyModel(partyModel);

    voterModel->setIdeologyModel(ideologyModel);
    partyModel->setIdeologyModel(ideologyModel);

    voterProxyModel = new QSortFilterProxyModel(this);

    // Models → Views
    ui->partyTableView->setModel(partyModel);
    voterProxyModel->setSourceModel(voterModel);
    voterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    voterProxyModel->setFilterKeyColumn(-1);
    ui->voterTableView->setModel(voterProxyModel);

    //Signals to autorefresh UI
    connect(partyModel, &PartyModel::partyAdded, partyModel, &PartyModel::reloadData);
    connect(partyModel, &PartyModel::partyUpdated, partyModel, &PartyModel::reloadData);
    connect(partyModel, &PartyModel::partyDeleted, partyModel, &PartyModel::reloadData);
    connect(partyModel, &PartyModel::partyUpdated, voterModel, &VoterModel::reloadData);

    connect(voterModel, &VoterModel::voterAdded, voterModel, &VoterModel::reloadData);
    connect(voterModel, &VoterModel::voterUpdated, voterModel, &VoterModel::reloadData);
    connect(voterModel, &VoterModel::voterDeleted, voterModel, &VoterModel::reloadData);

    // Recalculate chart on data change
    connect(voterModel, &VoterModel::voterAdded, partyModel, [&] {
        emit partyModel->dataChangedExternally();
    });
    connect(voterModel, &VoterModel::voterUpdated, partyModel, [&] {
        emit partyModel->dataChangedExternally();
    });
    connect(voterModel, &VoterModel::voterDeleted, partyModel, [&] {
        emit partyModel->dataChangedExternally();
    });

    //Tables allignment
    ui->partyTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->voterTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->partyTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->voterTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Tables row selection enforcment
    ui->partyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->voterTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->partyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->voterTableView->setSelectionMode(QAbstractItemView::SingleSelection);


    // Database
    QSqlDatabase db = QSqlDatabase::database("main_connection");
    partyModel->ensurePartiesPopulated(db);
    partyModel->reloadData();
    //partyModel->recalculatePopularityFromVoters(voterModel);

    // Create map of name → id for assigning to voters
    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel->rowCount(); ++i) {
        QModelIndex idx = partyModel->index(i, 0);
        QString name = partyModel->data(idx, Qt::DisplayRole).toString();
        int id = partyModel->data(idx, Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    voterModel->setPartyModel(partyModel);

    voterModel->ensureVotersPopulated(db, partyMap);
    voterModel->reloadData();
    //partyModel->recalculatePopularityFromVoters(voterModel);

    // Connect search bar to proxy
    connect(ui->voterSearchEdit, &QLineEdit::textChanged, this, [=](const QString &text) {
        voterProxyModel->setFilterFixedString(text);
    });

    //Charts Setup
    partyChart = new PartyChartWidget(partyModel, this);
    if (!ui->partyChartContainer->layout()) {
        auto *layout = new QVBoxLayout(ui->partyChartContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->partyChartContainer->setLayout(layout);
    }
    ui->partyChartContainer->layout()->addWidget(partyChart);
    partyChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->partyChartContainer->setMinimumSize(QSize(0, 0));

    voterChart = new VoterIdeologyChartWidget(this);
    voterChart->setVoterModel(voterModel);
    ui->partyChartContainer->layout()->addWidget(voterChart);

    voterFocusChart = new SingleVoterIdeologyWidget(this);
    ui->voterFocusWidget->setLayout(new QVBoxLayout());
    ui->voterFocusWidget->layout()->addWidget(voterFocusChart);

    connect(voterModel, &VoterModel::voterAdded, voterChart, &VoterIdeologyChartWidget::updateChart);
    connect(voterModel, &VoterModel::voterUpdated, voterChart, &VoterIdeologyChartWidget::updateChart);
    connect(voterModel, &VoterModel::voterDeleted, voterChart, &VoterIdeologyChartWidget::updateChart);

    connect(partyModel, &PartyModel::dataChangedExternally, partyChart, &PartyChartWidget::onDataChanged);

    setupButtonConnections();
}

void MainWindow::setupButtonConnections() {
    connect(ui->addPartyButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Add Party clicked";
        AddPartyDialog dialog(this);
        dialog.setIdeologyModel(ideologyModel);
        if (dialog.exec() == QDialog::Accepted) {
            partyModel->addParty(dialog.getParty());
        }
    });

    connect(ui->editPartyButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Edit Party clicked";
        QModelIndex index = ui->partyTableView->currentIndex();
        if (!index.isValid()) return;
        int id = partyModel->getPartyIdAt(index.row());
        Party party = partyModel->getPartyAt(index.row());

        AddPartyDialog dialog(this);
        dialog.setParty(party);
        dialog.setIdeologyModel(ideologyModel);
        dialog.setParty(party);
        if (dialog.exec() == QDialog::Accepted) {
            partyModel->updateParty(id, dialog.getParty());
        }
    });

    connect(ui->deletePartyButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Delete Party clicked";
        QModelIndex index = ui->partyTableView->currentIndex();
        if (!index.isValid()) return;
        int id = partyModel->getPartyIdAt(index.row());
        partyModel->deletePartyById(id);
    });

    connect(ui->addVoterButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Add Voter clicked";
        AddVoterDialog dialog(this, partyModel);
        dialog.setIdeologyModel(ideologyModel);
        if (dialog.exec() == QDialog::Accepted) {
            voterModel->addVoter(dialog.getVoter());
        }
        //partyModel->recalculatePopularityFromVoters(voterModel);
    });

    connect(ui->editVoterButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Edit Voter clicked";
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;
        int id = voterModel->getVoterIdAt(index.row());
        Voter voter = voterModel->getVoterAt(index.row());

        AddVoterDialog dialog(this, partyModel);
        dialog.setIdeologyModel(ideologyModel);
        dialog.setVoter(voter);
        dialog.setVoter(voter);
        if (dialog.exec() == QDialog::Accepted) {
            voterModel->updateVoter(id, dialog.getVoter());
        }
        //partyModel->recalculatePopularityFromVoters(voterModel);
    });

    connect(ui->deleteVoterButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "[UI] Delte Voter clicked";
        QModelIndex index = ui->voterTableView->currentIndex();
        if (!index.isValid()) return;
        int id = voterModel->getVoterIdAt(index.row());
        voterModel->deleteVoterById(id);
        //partyModel->recalculatePopularityFromVoters(voterModel);
    });

    connect(ui->voterTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &) {
        if (selected.indexes().isEmpty()) return;

        QModelIndex index = selected.indexes().first();
        int sourceRow = voterProxyModel->mapToSource(index).row();
        const Voter& voter = voterModel->getVoterAt(sourceRow);
        voterFocusChart->showVoter(voter);
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

    QSqlQuery resetVotersSeq(db);
    resetVotersSeq.exec("DELETE FROM sqlite_sequence WHERE name='voters'");

    QSqlQuery resetPartiesSeq(db);
    resetPartiesSeq.exec("DELETE FROM sqlite_sequence WHERE name='parties'");

    if (!clear.exec("DELETE FROM parties"))
        qWarning() << "[Reset] Failed to clear parties:" << clear.lastError().text();

    partyModel->ensurePartiesPopulated(db);
    partyModel->reloadData();
    //partyModel->recalculatePopularityFromVoters(voterModel);

    voterModel->setPartyModel(partyModel);

    QMap<QString, int> partyMap;
    for (int i = 0; i < partyModel->rowCount(); ++i) {
        QModelIndex index = partyModel->index(i, 0);
        QString name = partyModel->data(index, Qt::DisplayRole).toString();
        int id = partyModel->data(index, Qt::UserRole).toInt();
        partyMap.insert(name, id);
    }

    voterModel->ensureVotersPopulated(db, partyMap);
    voterModel->reloadData();
    //partyModel->recalculatePopularityFromVoters(voterModel);
}

MainWindow::~MainWindow()
{
    // Disconnect any remaining signals that might trigger DB usage
    disconnect(voterModel, nullptr, nullptr, nullptr);
    disconnect(partyModel, nullptr, nullptr, nullptr);

    // Delete UI first to ensure any widgets using models are gone
    delete ui;

    // Delete dependent widgets/models BEFORE closing DB
    delete voterFocusChart;
    delete voterChart;
    delete partyChart;
    delete voterProxyModel;

    delete voterModel;
    delete partyModel;

    // Now safely close and remove the DB connection
    if (QSqlDatabase::contains("main_connection")) {
        QSqlDatabase db = QSqlDatabase::database("main_connection");
        db.close();
        QSqlDatabase::removeDatabase("main_connection");
    }
}
