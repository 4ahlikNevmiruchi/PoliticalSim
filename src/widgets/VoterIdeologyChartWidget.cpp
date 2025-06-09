#include "VoterIdeologyChartWidget.h"
#include <QVBoxLayout>

VoterIdeologyChartWidget::VoterIdeologyChartWidget(QWidget *parent)
    : QWidget(parent), voterModel(nullptr)
{
    chart = new QChart();
    series = new QScatterSeries();
    series->setName("Voters");

    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisX->setRange(-100, 100);
    axisY->setRange(-100, 100);
    axisX->setTitleText("Left <--> Right");
    axisY->setTitleText("Libertarian <--> Authoritarian");

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->setTitle("Voter Ideology Distribution");
    chart->legend()->hide();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
}

void VoterIdeologyChartWidget::setVoterModel(VoterModel* model) {
    voterModel = model;
    updateChart();
}

void VoterIdeologyChartWidget::updateChart() {
    if (!voterModel) return;

    series->clear();
    for (int i = 0; i < voterModel->rowCount(); ++i) {
        const Voter& v = voterModel->getVoterAt(i);
        series->append(v.ideologyX, v.ideologyY);
    }
}
