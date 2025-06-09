#ifndef PARTYCHARTWIDGET_H
#define PARTYCHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include "models/PartyModel.h"

class PartyChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit PartyChartWidget(PartyModel *model, QWidget *parent = nullptr);

private:
    QChartView *chartView;
    QPieSeries *pieSeries;
    PartyModel *partyModel;

    void setupChart();
    void updateChart();

public slots:
    void onDataChanged();
};

#endif // PARTYCHARTWIDGET_H

