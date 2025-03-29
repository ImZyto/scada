#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

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

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;

    void updateStatus(const QString &text);
    void loadDummyFilters();
};

#endif // MAINWINDOW_H
