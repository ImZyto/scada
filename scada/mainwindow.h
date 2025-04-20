#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dataprocessor.h"
#include "filteraverage.h"
#include "filtersmooth.h"
#include "filtermedian.h"
#include <QMainWindow>
#include <QLabel>
#include "qcustomplot.h"
#include "dataclient.h"
#include <QVector>


struct FilterEntry {
    QString name;
    QCPGraph* graph;
    DataProcessor* processor;
};


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

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onApplyDisplayOptions();
    void onLoadFilter();
    void onRefreshFilters();
    void onDataReceived(const QByteArray &data);
    void onLegendItemDoubleClicked(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
    void on_sliderTimeline_valueChanged(int value);
    void on_actionDodajFiltr_triggered();
    void on_actionUsunFiltr_triggered();

private:
    QList<FilterEntry> activeFilters;

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;
    DataClient *dataClient;

    void updateStatus(const QString &text);
    void loadDummyFilters();

    QVector<QCPGraph*> graphs;
    DataProcessor* currentFilter;

    QVector<double> xData, yData;
    int dataCounter = 0;
    int currentXScale = 60;
    int currentYScale = 10;
    double lastYValue = 0.0;

    QMap<QString, QVector<QPair<double, double>>> dataBuffers;
    const int maxBufferSize = 1000;
    int currentSliderValue = 100;
    bool liveScroll = true;
};

#endif // MAINWINDOW_H
