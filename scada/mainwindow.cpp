#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "filteraverage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentFilter(nullptr)
{
    ui->setupUi(this);
    ui->labelPlotPlaceholder->hide();

    statusLabel = new QLabel("Status: Brak połączenia");
    ui->statusbar->addPermanentWidget(statusLabel);

    dataClient = new DataClient(this);

    ui->plot->xAxis->setLabel("Czas");
    ui->plot->yAxis->setLabel("Wartość");

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("HH:mm:ss");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->xAxis->setRange(QDateTime::currentSecsSinceEpoch() - 60, QDateTime::currentSecsSinceEpoch());

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

    connect(ui->actionPolaczZSerwerem, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(ui->actionRozlacz, &QAction::triggered, this, &MainWindow::onDisconnectClicked);
    connect(ui->actionZaladujFiltr, &QAction::triggered, this, &MainWindow::onLoadFilter);
    connect(ui->actionOdswiezFiltry, &QAction::triggered, this, &MainWindow::onRefreshFilters);
    connect(ui->actionZamknij, &QAction::triggered, this, &MainWindow::close);
    connect(ui->buttonApplyOptions, &QPushButton::clicked, this, &MainWindow::onApplyDisplayOptions);
    connect(ui->listFilters, &QListWidget::itemClicked, this, &MainWindow::onLoadFilter);

    loadDummyFilters();
}

MainWindow::~MainWindow()
{
    dataClient->disconnectFromServer();
    delete currentFilter;
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

    if (!graphs.isEmpty()) {
        graphs.last()->setPen(QPen(graphs.last()->pen().color(), 2, penStyle));
    }

    int xScale = ui->spinBoxScaleX->value();
    int yScale = ui->spinBoxScaleY->value();

    double now = QDateTime::currentSecsSinceEpoch();
    ui->plot->xAxis->setRange(now - xScale, now);
    ui->plot->yAxis->setRange(0, yScale);

    ui->plot->replot();

    ui->spinBoxScaleX->setValue(60);
    ui->spinBoxScaleY->setValue(10);
}

void MainWindow::onLoadFilter()
{
    QString selected = ui->listFilters->currentItem()->text();

    delete currentFilter;
    currentFilter = nullptr;

    QPen pen(Qt::black, 2);
    if (selected == "FilterAverage") {
        currentFilter = new FilterAverage(5);
        pen.setColor(Qt::blue);
    }
    else if (selected == "FilterSmooth") {
        currentFilter = new FilterSmooth(0.2);
        pen.setColor(Qt::darkGreen);
    }
    else if (selected == "FilterMedian") {
        currentFilter = new FilterMedian(5);
        pen.setColor(Qt::magenta);
    }

    QCPGraph* newGraph = ui->plot->addGraph();
    newGraph->setPen(pen);
    newGraph->setLineStyle(QCPGraph::lsLine);
    newGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    graphs.append(newGraph);
    currentX.clear();
    currentY.clear();

    ui->plot->replot();
}

void MainWindow::onRefreshFilters()
{
    loadDummyFilters();
    QMessageBox::information(this, "Filtry", "Odświeżono listę filtrów.");
}

void MainWindow::loadDummyFilters()
{
    ui->listFilters->clear();
    ui->listFilters->addItem("Brak");
    ui->listFilters->addItem("FilterSmooth");
    ui->listFilters->addItem("FilterMedian");
    ui->listFilters->addItem("FilterAverage");
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString message = QString::fromUtf8(data).trimmed();
    qDebug() << "Odebrano dane:" << message;

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

                if (currentFilter)
                    y = currentFilter->processSample(y);

                currentX.append(x);
                currentY.append(y);

                if (currentX.size() > 1000) {
                    currentX.removeFirst();
                    currentY.removeFirst();
                }

                if (!graphs.isEmpty()) {
                    graphs.last()->addData(x, y);
                    ui->plot->xAxis->setRange(x - 60, x);
                    ui->plot->yAxis->rescale();
                    ui->plot->replot();
                }
            }
        }
    }
}
