#include "dataclient.h"
#include <QDebug>

DataClient::DataClient(QObject *parent)
    : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &DataClient::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &DataClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &DataClient::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &DataClient::onError);
}

void DataClient::connectToServer(const QString &host, quint16 port)
{
    socket->connectToHost(host, port);
}

void DataClient::disconnectFromServer()
{
    socket->disconnectFromHost();
}

void DataClient::onConnected()
{
    emit connected();
}

void DataClient::onDisconnected()
{
    emit disconnected();
}

void DataClient::onReadyRead()
{
    QByteArray data = socket->readAll();
    emit dataReceived(data);
}

void DataClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorOccurred(socket->errorString());
}
