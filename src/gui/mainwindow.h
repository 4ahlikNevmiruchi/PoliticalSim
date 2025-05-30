#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "models/PartyModel.h"
#include "models/VoterModel.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Main application window
 * 
 * @details Displays party data in a table view and handles user interactions
 * such as adding new parties.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    PartyModel* partyModel;
    VoterModel *voterModel;
};

#endif // MAINWINDOW_H
