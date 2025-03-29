#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Status bar
    statusLabel = new QLabel("Status: Brak połączenia");
    ui->statusbar->addPermanentWidget(statusLabel);

    // Inicjalizacja klienta TCP
    dataClient = new DataClient(this);

    // Połączenia sygnałów z DataClient
    connect(dataClient, &DataClient::connected, this, [this]() {
        updateStatus("Połączono z serwerem");
    });

    connect(dataClient, &DataClient::disconnected, this, [this]() {
        updateStatus("Rozłączono");
    });

    connect(dataClient, &DataClient::dataReceived, this, [this](const QByteArray &data) {
        qDebug() << "Odebrano dane:" << data;
    });

    connect(dataClient, &DataClient::errorOccurred, this, [this](const QString &error) {
        QMessageBox::warning(this, "Błąd połączenia", error);
    });

    // Podłączenie akcji menu
    connect(ui->actionPolaczZSerwerem, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(ui->actionRozlacz, &QAction::triggered, this, &MainWindow::onDisconnectClicked);
    connect(ui->actionZaladujFiltr, &QAction::triggered, this, &MainWindow::onLoadFilter);
    connect(ui->actionOdswiezFiltry, &QAction::triggered, this, &MainWindow::onRefreshFilters);

    // Przycisk Zastosuj
    connect(ui->buttonApplyOptions, &QPushButton::clicked, this, &MainWindow::onApplyDisplayOptions);

    // Dummy data
    loadDummyFilters();
}

MainWindow::~MainWindow()
{
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
    QMessageBox::information(this, "Opcje", "Zastosowano ustawienia wyświetlania.");
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
