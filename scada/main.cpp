#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);

    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);

    QFile file(logDir + "/test_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ");
        out << msg << "\n";
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
    qDebug() << "TEST: The application has started!";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
