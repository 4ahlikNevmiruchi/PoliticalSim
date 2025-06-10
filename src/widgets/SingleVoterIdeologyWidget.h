#ifndef SINGLEVOTERIDEOLOGYWIDGET_H
#define SINGLEVOTERIDEOLOGYWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "models/Voter.h"

/**
 * @brief Widget for displaying a single voter's ideology on a 2D chart.
 */
class SingleVoterIdeologyWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a SingleVoterIdeologyWidget.
     * @param parent Optional parent widget.
     *
     * Initializes the chart for displaying a single voter's ideology.
     */
    explicit SingleVoterIdeologyWidget(QWidget *parent = nullptr);

    /**
     * @brief Plots the given voter's ideological position on the chart.
     * @param voter The Voter whose ideology will be displayed.
     *
     * Resets the chart series to show the provided voter's coordinates.
     */
    void showVoter(const Voter& voter);

private:
    QChart* chart;               ///< Chart object for the ideology scatter plot.
    QChartView* chartView;         ///< View widget for displaying the chart.
    QScatterSeries* series;         ///< Scatter series representing the single voter point.
    QValueAxis* axisX;             ///< X-axis (economic axis) of the chart.
    QValueAxis* axisY;             ///< Y-axis (social axis) of the chart.
};

#endif // SINGLEVOTERIDEOLOGYWIDGET_H
