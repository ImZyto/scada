#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Dodanie status label do statusbar
    statusLabel = new QLabel("Status: Brak połączenia");
    ui->statusbar->addPermanentWidget(statusLabel);

    // Podłączenie akcji menu
    connect(ui->actionPolaczZSerwerem, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(ui->actionRozlacz, &QAction::triggered, this, &MainWindow::onDisconnectClicked);
    connect(ui->actionZaladujFiltr, &QAction::triggered, this, &MainWindow::onLoadFilter);
    connect(ui->actionOdswiezFiltry, &QAction::triggered, this, &MainWindow::onRefreshFilters);

    // Podłączenie przycisku Zastosuj
    connect(ui->buttonApplyOptions, &QPushButton::clicked, this, &MainWindow::onApplyDisplayOptions);

    // Załaduj przykładowe filtry
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
    updateStatus("Połączono z 127.0.0.1:1234");
}

void MainWindow::onDisconnectClicked()
{
    updateStatus("Rozłączono");
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
