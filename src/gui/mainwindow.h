#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "models/PartyModel.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PartyModel* partyModel;
};

#endif // MAINWINDOW_H
