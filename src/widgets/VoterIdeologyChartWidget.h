#ifndef VOTERIDEOLOGYCHARTWIDGET_H
#define VOTERIDEOLOGYCHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "models/VoterModel.h"

/**
 * @brief Widget for displaying all voters on an ideology scatter plot.
 */
class VoterIdeologyChartWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a VoterIdeologyChartWidget.
     * @param parent Optional parent widget.
     *
     * Initializes the scatter plot chart for all voters.
     */
    explicit VoterIdeologyChartWidget(QWidget *parent = nullptr);

    /**
     * @brief Assigns a VoterModel to this widget.
     * @param model Pointer to the VoterModel providing voter data.
     *
     * Connects the model's signals to update the chart when data changes.
     */
    void setVoterModel(VoterModel* model);

    /**
     * @brief Updates the scatter plot to reflect the current voter data.
     *
     * Re-populates the chart series using all voters from the model.
     */
    void updateChart();

private:
    QChart* chart;               ///< Chart object for plotting voter ideologies.
    QChartView* chartView;         ///< View widget for the scatter chart.
    QScatterSeries* series;         ///< Scatter series representing all voter points.
    QValueAxis* axisX;             ///< X-axis (economic axis) of the chart.
    QValueAxis* axisY;             ///< Y-axis (social axis) of the chart.
    VoterModel* voterModel;         ///< VoterModel providing data for the chart.
};

#endif // VOTERIDEOLOGYCHARTWIDGET_H
