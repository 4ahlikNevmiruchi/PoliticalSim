#include "PartyChartWidget.h"
#include "PartyModel.h"
#include <QtCharts/QChart>
#include <QVBoxLayout>

PartyChartWidget::PartyChartWidget(PartyModel *model, QWidget *parent)
    : QWidget(parent), partyModel(model) {

    pieSeries = new QPieSeries();
    setupChart();
    updateChart();

    connect(partyModel, &PartyModel::dataChangedExternally, this, &PartyChartWidget::onDataChanged);
    connect(partyModel, &PartyModel::popularityRecalculated, this, &PartyChartWidget::onDataChanged);

}

void PartyChartWidget::setupChart() {
    QChart *chart = new QChart();
    chart->addSeries(pieSeries);
    chart->setTitle("Party Popularity");
    chart->legend()->setAlignment(Qt::AlignRight);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    layout->setContentsMargins(0, 0, 0, 0);
}

void PartyChartWidget::onDataChanged() {
    updateChart();
}

void PartyChartWidget::updateChart() {
    pieSeries->clear();
    const QVector<Party>& parties = partyModel->getAllParties();

    double total = 0;
    for (const Party &p : parties) {
        double pct = partyModel->calculatePopularity(p.id);
        if (pct > 0) {
            total += pct;
        }
    }

    if (total == 0) {
        pieSeries->append("No Data", 1.0);
        return;
    }

    for (const Party &p : parties) {
        double pct = partyModel->calculatePopularity(p.id);
        if (pct > 0) {
            pieSeries->append(p.name, pct);
        }
    }
}
