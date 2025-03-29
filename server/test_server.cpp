#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <QRandomGenerator>

class TestServer : public QObject
{
    Q_OBJECT
public:
    TestServer(QObject *parent = nullptr) : QObject(parent)
    {
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &TestServer::handleNewConnection);

        if (server->listen(QHostAddress::Any, 1234)) {
            qDebug() << "Serwer nasłuchuje na porcie 1234";
        } else {
            qDebug() << "Błąd przy uruchamianiu serwera:" << server->errorString();
        }
    }

private slots:
    void handleNewConnection()
    {
        socket = server->nextPendingConnection();
        qDebug() << "Nowe połączenie z" << socket->peerAddress().toString();

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

        // Co sekundę wysyłaj dane
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() {
            if (socket->state() == QAbstractSocket::ConnectedState) {
                QString message = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") +
                                  ", " + QString::number(QRandomGenerator::global()->bounded(10.0), 'f', 2) + "\n";
                socket->write(message.toUtf8());
                socket->flush();
            }
        });
        timer->start(1000);
    }

private:
    QTcpServer *server;
    QTcpSocket *socket = nullptr;
};

#include "test_server.moc"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestServer server;

    return a.exec();
}
