#include "SingleVoterIdeologyWidget.h"
#include <QVBoxLayout>

SingleVoterIdeologyWidget::SingleVoterIdeologyWidget(QWidget* parent)
    : QWidget(parent)
{
    chart = new QChart();
    chart->setTitle("Selected Voter Ideology");

    series = new QScatterSeries();
    series->setMarkerSize(12.0);
    chart->addSeries(series);

    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisX->setRange(-100, 100);
    axisY->setRange(-100, 100);
    axisX->setTitleText("Left <--> Right");
    axisY->setTitleText("Libertarian <--> Authoritarian");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->legend()->hide();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
}

void SingleVoterIdeologyWidget::showVoter(const Voter& voter) {
    series->clear();
    series->append(voter.ideologyX, voter.ideologyY);
}