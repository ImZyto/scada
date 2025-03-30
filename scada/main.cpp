#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);

    QFile file("C:/Users/micha/Desktop/test_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ");
        out << msg << "\n";
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler); // <<< TO JEST KLUCZOWE
    qDebug() << "TEST: Aplikacja wystartowała!";
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
