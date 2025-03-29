#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "filteraverage.h"
#include "filtersmooth.h"
#include "filtermedian.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentFilter(nullptr)
{
    ui->setupUi(this);
    ui->spinBoxScaleX->setValue(40);
    ui->spinBoxScaleY->setValue(10);

    currentXScale = 40;
    currentYScale = 10;

    double now = QDateTime::currentSecsSinceEpoch();
    ui->plot->xAxis->setRange(now - currentXScale, now);
    ui->plot->yAxis->setRange(0, currentYScale);

    ui->labelPlotPlaceholder->hide();

    statusLabel = new QLabel("Status: Brak połączenia");
    ui->statusbar->addPermanentWidget(statusLabel);

    dataClient = new DataClient(this);

    // Konfiguracja wykresu
    ui->plot->xAxis->setLabel("Czas");
    ui->plot->yAxis->setLabel("Wartość");

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("HH:mm:ss");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->xAxis->setRange(QDateTime::currentSecsSinceEpoch() - 60, QDateTime::currentSecsSinceEpoch());

    // Połączenia
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
    connect(ui->plot, &QCustomPlot::legendDoubleClick, this, &MainWindow::onLegendItemDoubleClicked);

    loadDummyFilters();

    // Ładuj domyślny filtr „Brak”
    QListWidgetItem* firstItem = ui->listFilters->item(0);
    if (firstItem) {
        ui->listFilters->setCurrentItem(firstItem);
        onLoadFilter();


    }
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
    currentXScale = ui->spinBoxScaleX->value();

    bool gridOn = ui->checkBoxGrid->isChecked();
    ui->plot->xAxis->grid()->setVisible(gridOn);
    ui->plot->yAxis->grid()->setVisible(gridOn);

    // Styl linii
    int styleIndex = ui->comboBoxLineStyle->currentIndex();
    Qt::PenStyle penStyle = Qt::SolidLine;

    switch (styleIndex) {
    case 0: penStyle = Qt::SolidLine; break;
    case 1: penStyle = Qt::DotLine; break;
    case 2: penStyle = Qt::DashLine; break;
    }

    if (!activeFilters.isEmpty()) {
        activeFilters.last().graph->setPen(QPen(activeFilters.last().graph->pen().color(), 2, penStyle));
    }

    // Zakres X
    int xScale = ui->spinBoxScaleX->value();
    if (!activeFilters.isEmpty()) {
        QCPGraph* lastGraph = activeFilters.last().graph;
        if (!lastGraph->data()->isEmpty()) {
            double lastX = (--(lastGraph->data()->constEnd()))->key;
            ui->plot->xAxis->setRange(lastX - xScale, lastX);
        }
    }

    // Zakres Y tylko jeśli faktycznie zmieniony
    int newYScale = ui->spinBoxScaleY->value();
    if (newYScale != currentYScale) {
        currentYScale = newYScale;
        if (currentYScale > 0) {
            ui->plot->yAxis->setRange(lastYValue - currentYScale / 2.0, lastYValue + currentYScale / 2.0);
        } else {
            ui->plot->yAxis->rescale();
        }
    }

    ui->plot->replot();
}

void MainWindow::onLoadFilter()
{
    if (!ui->listFilters->currentItem()) return;

    QString selected = ui->listFilters->currentItem()->text();

    delete currentFilter;
    currentFilter = nullptr;

    DataProcessor* processor = nullptr;
    QPen pen(Qt::black, 2);

    if (selected == "FilterAverage") {
        processor = new FilterAverage(5);
        pen.setColor(Qt::blue);
    } else if (selected == "FilterSmooth") {
        processor = new FilterSmooth(0.2);
        pen.setColor(Qt::darkGreen);
    } else if (selected == "FilterMedian") {
        processor = new FilterMedian(5);
        pen.setColor(Qt::magenta);
    } else if (selected == "Brak") {
        processor = nullptr;
        pen.setColor(Qt::red);
    }

    QCPGraph* newGraph = ui->plot->addGraph();
    static QMap<QString, int> filterCounts;
    int count = ++filterCounts[selected];
    newGraph->setName(QString("%1 #%2").arg(selected).arg(count));
    newGraph->setPen(pen);
    newGraph->setLineStyle(QCPGraph::lsLine);
    newGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    activeFilters.append({ selected, newGraph, processor });

    ui->plot->legend->setVisible(true);
    ui->plot->legend->setFont(QFont("Helvetica", 9));
    ui->plot->legend->setBrush(QBrush(Qt::white));
    ui->plot->replot();

    switch (newGraph->pen().style()) {
    case Qt::SolidLine:
        ui->comboBoxLineStyle->setCurrentIndex(0);
        break;
    case Qt::DotLine:
        ui->comboBoxLineStyle->setCurrentIndex(1);
        break;
    case Qt::DashLine:
        ui->comboBoxLineStyle->setCurrentIndex(2);
        break;
    default:
        ui->comboBoxLineStyle->setCurrentIndex(0);
    }
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

    if (activeFilters.isEmpty()) {
        qWarning() << "Brak aktywnego wykresu – najpierw wybierz filtr!";
        return;
    }

    QStringList parts = message.split(',');
    if (parts.size() == 2) {
        QString timestampStr = parts[0].trimmed();
        QString valueStr = parts[1].trimmed();

        QDateTime timestamp = QDateTime::fromString(timestampStr, "yyyy-MM-dd HH:mm:ss");
        if (timestamp.isValid()) {
            bool ok;
            double value = valueStr.toDouble(&ok);
            lastYValue = value;
            if (ok) {
                double x = timestamp.toSecsSinceEpoch();

                for (auto& entry : activeFilters) {
                    double yVal = value;
                    if (entry.processor)
                        yVal = entry.processor->processSample(value);

                    entry.graph->addData(x, yVal);

                    if (entry.graph->data()->size() > 1000)
                        entry.graph->data()->removeBefore(x - 300);
                }

                ui->plot->xAxis->setRange(x - currentXScale, x);
                if (currentYScale > 0) {
                    // Nic nie zmieniaj – użytkownik już ustawił zakres ręcznie
                } else {
                    ui->plot->yAxis->rescale();
                }
                ui->plot->replot();
            }
        }
    }
}

void MainWindow::onLegendItemDoubleClicked(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)
{
    Q_UNUSED(legend);
    Q_UNUSED(event);

    if (!item) return;

    QCPPlottableLegendItem *legendItem = qobject_cast<QCPPlottableLegendItem*>(item);
    if (!legendItem) return;

    QCPGraph *graph = qobject_cast<QCPGraph*>(legendItem->plottable());
    if (!graph) return;

    // Usuń wykres i jego filtr
    ui->plot->removeGraph(graph);

    for (int i = 0; i < activeFilters.size(); ++i) {
        if (activeFilters[i].graph == graph) {
            delete activeFilters[i].processor;
            activeFilters.removeAt(i);
            break;
        }
    }

    ui->plot->replot();
}
