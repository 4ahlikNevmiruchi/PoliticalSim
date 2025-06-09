#ifndef SINGLEVOTERIDEOLOGYWIDGET_H
#define SINGLEVOTERIDEOLOGYWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "models/Voter.h"

class SingleVoterIdeologyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SingleVoterIdeologyWidget(QWidget* parent = nullptr);
    void showVoter(const Voter& voter);

private:
    QChart* chart;
    QChartView* chartView;
    QScatterSeries* series;
    QValueAxis* axisX;
    QValueAxis* axisY;
};

#endif // SINGLEVOTERIDEOLOGYWIDGET_H
