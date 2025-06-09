#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "models/PartyModel.h"
#include "models/VoterModel.h"

#include "widgets/VoterIdeologyChartWidget.h"
#include "widgets/SingleVoterIdeologyWidget.h"
#include "widgets/PartyChartWidget.h"

#include <QSortFilterProxyModel>


namespace Ui {
class MainWindow;
}

/**
 * @brief Main application window for PoliticalSim.
 *
 * @details The MainWindow sets up the UI, initializes the Party and Voter models,
 * populates default data if needed, and manages user interactions (adding parties/voters, reset).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main application window.
     * @param parent Optional parent widget.
     *
     * Sets up UI components and models, and ensures the database is initialized with default
     * data if empty. Also connects UI signals (buttons, etc.) to their respective slots.
     */
    explicit MainWindow(QWidget *parent = nullptr);
    /** @brief Destructor for MainWindow. Cleans up the UI and models. */
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    PartyModel* partyModel;
    VoterModel *voterModel;
    IdeologyModel *ideologyModel;
    QSortFilterProxyModel* voterProxyModel;
    void setupButtonConnections();
    void resetDatabase();

    VoterIdeologyChartWidget* voterChart;
    SingleVoterIdeologyWidget* voterFocusChart;
    PartyChartWidget *partyChart;
};

#endif // MAINWINDOW_H
