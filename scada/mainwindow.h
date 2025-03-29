#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "qcustomplot.h"
#include "dataclient.h"

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
    void onDataReceived(const QByteArray &data);  // <- tu dodane do slotów

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;
    DataClient *dataClient;

    void updateStatus(const QString &text);
    void loadDummyFilters();

    QVector<double> xData, yData;  // dane do wykresu
    int dataCounter = 0;
};

#endif // MAINWINDOW_H
