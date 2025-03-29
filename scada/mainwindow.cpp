#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->labelPlotPlaceholder->hide();

    // Status bar
    statusLabel = new QLabel("Status: Brak połączenia");
    ui->statusbar->addPermanentWidget(statusLabel);

    // Inicjalizacja klienta TCP
    dataClient = new DataClient(this);

    // Inicjalizacja wykresu
    ui->plot->addGraph();
    ui->plot->xAxis->setLabel("Czas");
    ui->plot->yAxis->setLabel("Wartość");

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("HH:mm:ss");
    ui->plot->xAxis->setTicker(timeTicker);

    ui->plot->xAxis->setRange(QDateTime::currentSecsSinceEpoch() - 60, QDateTime::currentSecsSinceEpoch());

    ui->plot->graph(0)->setPen(QPen(Qt::blue, 2));
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    // Połączenia sygnałów z DataClient
    connect(dataClient, &DataClient::connected, this, [this]() {
        updateStatus("Połączono z serwerem");
    });

    connect(dataClient, &DataClient::disconnected, this, [this]() {
        updateStatus("Rozłączono");
    });

    connect(dataClient, &DataClient::dataReceived, this, &MainWindow::onDataReceived);

    connect(dataClient, &DataClient::errorOccurred, this, [this](const QString &error) {
        QMessageBox::warning(this, "Błąd połączenia", error);
    });

    // Akcje menu
    connect(ui->actionPolaczZSerwerem, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(ui->actionRozlacz, &QAction::triggered, this, &MainWindow::onDisconnectClicked);
    connect(ui->actionZaladujFiltr, &QAction::triggered, this, &MainWindow::onLoadFilter);
    connect(ui->actionOdswiezFiltry, &QAction::triggered, this, &MainWindow::onRefreshFilters);

    // Przycisk Zastosuj
    connect(ui->buttonApplyOptions, &QPushButton::clicked, this, &MainWindow::onApplyDisplayOptions);

    loadDummyFilters();
}

MainWindow::~MainWindow()
{
    dataClient->disconnectFromServer();
    delete ui;
}

void MainWindow::updateStatus(const QString &text)
{
    statusLabel->setText("Status: " + text);
}

void MainWindow::onConnectClicked()
{
    dataClient->connectToServer("127.0.0.1", 1234);
}

void MainWindow::onDisconnectClicked()
{
    dataClient->disconnectFromServer();
}

void MainWindow::onApplyDisplayOptions()
{
    bool gridOn = ui->checkBoxGrid->isChecked();
    ui->plot->xAxis->grid()->setVisible(gridOn);
    ui->plot->yAxis->grid()->setVisible(gridOn);

    int styleIndex = ui->comboBoxLineStyle->currentIndex();
    Qt::PenStyle penStyle = Qt::SolidLine;

    switch (styleIndex) {
    case 0: penStyle = Qt::SolidLine; break;
    case 1: penStyle = Qt::DotLine; break;
    case 2: penStyle = Qt::DashLine; break;
    }

    ui->plot->graph(0)->setPen(QPen(Qt::blue, 2, penStyle));

    int xScale = ui->spinBoxScaleX->value();
    int yScale = ui->spinBoxScaleY->value();

    double now = QDateTime::currentSecsSinceEpoch();
    ui->plot->xAxis->setRange(now - xScale, now);
    ui->plot->yAxis->setRange(0, yScale);

    ui->plot->replot();

    // Reset spinnerów
    ui->spinBoxScaleX->setValue(60);
    ui->spinBoxScaleY->setValue(10);
}

void MainWindow::onLoadFilter()
{
    QMessageBox::information(this, "Filtry", "Załadowano filtr.");
}

void MainWindow::onRefreshFilters()
{
    loadDummyFilters();
    QMessageBox::information(this, "Filtry", "Odświeżono listę filtrów.");
}

void MainWindow::loadDummyFilters()
{
    ui->listFilters->clear();
    ui->listFilters->addItem("FilterSmooth");
    ui->listFilters->addItem("FilterMedian");
    ui->listFilters->addItem("FilterAverage");
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString message = QString::fromUtf8(data).trimmed();
    qDebug() << "Odebrano dane:" << message;

    // Oczekiwany format: "2025-03-29 14:10:12, 4.37"
    QStringList parts = message.split(',');
    if (parts.size() == 2) {
        QString timestampStr = parts[0].trimmed();
        QString valueStr = parts[1].trimmed();

        QDateTime timestamp = QDateTime::fromString(timestampStr, "yyyy-MM-dd HH:mm:ss");
        if (timestamp.isValid()) {
            bool ok;
            double value = valueStr.toDouble(&ok);
            if (ok) {
                double x = timestamp.toSecsSinceEpoch();
                double y = value;

                // Dodaj nowe dane do bufora
                xData.append(x);
                yData.append(y);

                // Usuń dane starsze niż 5 minut (300 sekund)
                while (!xData.isEmpty() && xData.first() < x - 300) {
                    xData.removeFirst();
                    yData.removeFirst();
                }

                // Aktualizuj wykres
                ui->plot->graph(0)->setData(xData, yData);
                ui->plot->xAxis->setRange(x - 60, x); // przesuwanie wykresu
                ui->plot->yAxis->rescale();           // dopasowanie osi Y
                ui->plot->replot();
            }
        }
    }
}
