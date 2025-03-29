#ifndef DATACLIENT_H
#define DATACLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class DataClient : public QObject
{
    Q_OBJECT
public:
    explicit DataClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
};

#endif // DATACLIENT_H
