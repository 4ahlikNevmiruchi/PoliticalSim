#ifndef PARTYCHARTWIDGET_H
#define PARTYCHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include "models/PartyModel.h"

/**
 * @brief Widget for displaying party popularity as a pie chart.
 */
class PartyChartWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a PartyChartWidget.
     * @param model Pointer to the PartyModel providing party data.
     * @param parent Optional parent widget.
     *
     * Initializes the pie chart components and connects to the model.
     */
    explicit PartyChartWidget(PartyModel *model, QWidget *parent = nullptr);

public slots:
    /**
     * @brief Slot to update the chart when the party data changes.
     *
     * Rebuilds the pie chart to reflect the current party data.
     */
    void onDataChanged();

private:
    void setupChart();       ///< Sets up the chart view and pie series.
    void updateChart();      ///< Refreshes the pie chart with the current party data.

    QChartView* chartView;       ///< Chart view widget displaying the pie chart.
    QPieSeries* pieSeries;       ///< Pie series representing party popularity data.
    PartyModel* partyModel;      ///< PartyModel providing the data for the chart.
};

#endif // PARTYCHARTWIDGET_H
