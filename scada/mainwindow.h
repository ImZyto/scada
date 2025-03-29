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

    QVector<double> xData, yData;  // dane do wykresu
    int dataCounter = 0;
};

#endif // MAINWINDOW_H
