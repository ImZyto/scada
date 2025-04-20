#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "filteraverage.h"
#include "filtersmooth.h"
#include "filtermedian.h"
#include <QInputDialog>


QStringList availableFilterTypes = {
    "FilterAverage",
    "FilterSmooth",
    "FilterMedian"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentFilter(nullptr)
{
    ui->setupUi(this);
    ui->listFilters->clear();
    ui->listFilters->addItem("Brak");
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

    ui->plot->xAxis->grid()->setVisible(false);
    ui->plot->yAxis->grid()->setVisible(false);
    bool isGridVisible = ui->plot->xAxis->grid()->visible();
    ui->checkBoxGrid->setChecked(isGridVisible);

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

    connect(ui->actionDodajFiltr, &QAction::triggered, this, &MainWindow::on_actionDodajFiltr_triggered);
    connect(ui->actionOdswiezFiltry, &QAction::triggered, this, &MainWindow::onRefreshFilters);
    connect(ui->actionZamknij, &QAction::triggered, this, &MainWindow::close);
    connect(ui->buttonApplyOptions, &QPushButton::clicked, this, &MainWindow::onApplyDisplayOptions);
    connect(ui->listFilters, &QListWidget::itemClicked, this, &MainWindow::onLoadFilter);
    connect(ui->plot, &QCustomPlot::legendDoubleClick, this, &MainWindow::onLegendItemDoubleClicked);

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
    QString defaultIp = "127.0.0.1";
    int defaultPort = 1234;

    bool okIp;
    QString ip = QInputDialog::getText(this, "Adres IP",
                                       "Wprowadź adres IP serwera:",
                                       QLineEdit::Normal, defaultIp, &okIp);

    if (!okIp || ip.isEmpty())
        return;

    bool okPort;
    int port = QInputDialog::getInt(this, "Port",
                                    "Wprowadź port serwera:",
                                    defaultPort, 1, 65535, 1, &okPort);

    if (!okPort)
        return;

    dataClient->connectToServer(ip, port);
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

    if (!activeFilters.isEmpty()) {
        QCPGraph* lastGraph = activeFilters.last().graph;
        if (!lastGraph->data()->isEmpty()) {
            double lastX = (--(lastGraph->data()->constEnd()))->key;

            if (liveScroll) {
                ui->plot->xAxis->setRange(lastX - currentXScale, lastX);
            } else {

            }
        }
    }

    currentYScale = ui->spinBoxScaleY->value();
    if (currentYScale > 0) {
        ui->plot->yAxis->setRange(0, currentYScale);
    } else {
        ui->plot->yAxis->rescale();
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

bool isGraphStillInPlot(QCustomPlot* plot, QCPGraph* graphToFind)
{
    int count = plot->graphCount();
    for (int i = 0; i < count; ++i) {
        if (plot->graph(i) == graphToFind)
            return true;
    }
    return false;
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
                    if (!entry.graph || !isGraphStillInPlot(ui->plot, entry.graph))
                        continue;

                    double yVal = value;
                    if (entry.processor)
                        yVal = entry.processor->processSample(value);

                    auto &buffer = dataBuffers[entry.name];
                    buffer.append(qMakePair(x, yVal));
                    if (buffer.size() > maxBufferSize)
                        buffer.remove(0, buffer.size() - maxBufferSize);

                    entry.graph->addData(x, yVal);
                    QVector<double> xData, yData;
                    for (const auto &pair : buffer) {
                        xData.append(pair.first);
                        yData.append(pair.second);
                    }
                    entry.graph->data()->removeBefore(x - 300);
                }

                if (liveScroll) {
                    ui->plot->xAxis->setRange(x - currentXScale, x);
                }
                if (currentYScale > 0) {

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

    QString graphName = graph->name();
    ui->plot->removeGraph(graph);

    auto it = std::find_if(activeFilters.begin(), activeFilters.end(),
                           [graph](const FilterEntry &entry) {
                               return entry.graph == graph;
                           });

    if (it != activeFilters.end()) {
        delete it->processor;
        dataBuffers.remove(graphName);
        activeFilters.erase(it);
    }

    ui->plot->replot();
    if (activeFilters.isEmpty()) {
        ui->plot->xAxis->setRange(QDateTime::currentSecsSinceEpoch() - currentXScale,
                                  QDateTime::currentSecsSinceEpoch());
        ui->plot->replot();
    }
    return;
}

void MainWindow::on_sliderTimeline_valueChanged(int value)
{
    currentSliderValue = value;
    liveScroll = (value == 100);

    if (activeFilters.isEmpty()) return;

    auto &buffer = dataBuffers[activeFilters.last().name];
    if (buffer.isEmpty()) return;

    double lastX = buffer.last().first;

    double offset = (100 - value) * (currentXScale / 100.0);
    ui->plot->xAxis->setRange(lastX - currentXScale - offset, lastX - offset);
    ui->plot->replot();
}

void MainWindow::on_actionDodajFiltr_triggered()
{
    QStringList availableFilters = { "FilterSmooth", "FilterMedian", "FilterAverage" };

    for (int i = 0; i < ui->listFilters->count(); ++i) {
        availableFilters.removeAll(ui->listFilters->item(i)->text());
    }

    if (availableFilters.isEmpty()) {
        QMessageBox::information(this, "Brak filtrów", "Wszystkie dostępne filtry zostały już dodane.");
        return;
    }

    bool ok;
    QString chosen = QInputDialog::getItem(this, "Dodaj filtr",
                                           "Wybierz filtr do dodania:",
                                           availableFilters, 0, false, &ok);

    if (ok && !chosen.isEmpty()) {
        ui->listFilters->addItem(chosen);
        QMessageBox::information(this, "Dodano", "Filtr \"" + chosen + "\" został dodany.");
    }
}


void MainWindow::on_actionUsunFiltr_triggered()
{
    if (!ui->listFilters->currentItem()) {
        QMessageBox::warning(this, "Brak zaznaczenia", "Wybierz filtr do usunięcia z listy.");
        return;
    }

    QString selected = ui->listFilters->currentItem()->text();

    for (const auto& entry : activeFilters) {
        if (entry.name == selected) {
            QMessageBox::warning(this, "Nie można usunąć",
                                 "Ten filtr jest obecnie aktywny na wykresie. Usuń go z wykresu najpierw (kliknij dwukrotnie w legendę).");
            return;
        }
    }

    delete ui->listFilters->takeItem(ui->listFilters->currentRow());
    QMessageBox::information(this, "Usunięto", "Filtr \"" + selected + "\" został usunięty.");
}


