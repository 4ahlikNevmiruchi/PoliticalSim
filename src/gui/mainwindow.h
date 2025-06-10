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
 * @brief Main window of the PoliticalSim application.
 *
 * @details Initializes the user interface and all data models (Party, Voter, Ideology). If the database is empty, it populates default data. It also manages user interactions such as adding, editing, or deleting parties and voters, resetting the database, and updates all associated charts and tables accordingly.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main application window.
     * @param parent Optional parent widget.
     *
     * Sets up the user interface, data models, proxy models, and chart widgets. Also connects signals to the UI components and populates initial data if the database is empty.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /** @brief Destructor. Releases allocated resources. */
    ~MainWindow();

private:
    Ui::MainWindow *ui;                                 ///< Auto-generated UI form pointer.

    PartyModel* partyModel;                             ///< Pointer to the PartyModel.
    VoterModel* voterModel;                             ///< Pointer to the VoterModel.
    IdeologyModel* ideologyModel;                       ///< Pointer to the IdeologyModel.

    QSortFilterProxyModel* voterProxyModel;             ///< Proxy for filtering/searching voters.
    void setupButtonConnections();                      ///< Connects all toolbar and button signals.
    void resetDatabase();                               ///< Resets all data to the built-in defaults.

    VoterIdeologyChartWidget* voterChart;               ///< Scatter-chart widget for voter ideology distribution.
    SingleVoterIdeologyWidget* voterFocusChart;         ///< Scatter-chart widget for the selected voter.
    PartyChartWidget* partyChart;                       ///< Pie-chart widget for party popularity.
};

#endif // MAINWINDOW_H
