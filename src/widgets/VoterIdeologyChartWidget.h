#ifndef VOTERIDEOLOGYCHARTWIDGET_H
#define VOTERIDEOLOGYCHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "models/VoterModel.h"



class VoterIdeologyChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit VoterIdeologyChartWidget(QWidget *parent = nullptr);
    void setVoterModel(VoterModel* model);
    void updateChart();

private:
    QChart* chart;
    QChartView* chartView;
    QScatterSeries* series;
    QValueAxis* axisX;
    QValueAxis* axisY;
    VoterModel* voterModel;
};

#endif // VOTERIDEOLOGYCHARTWIDGET_H
