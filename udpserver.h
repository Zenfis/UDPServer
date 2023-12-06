#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QWidget>
#include <QUdpSocket>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QTime>
#include <QSlider>
#include <QVBoxLayout>
#include <QNetworkProxy>
#include <QSettings>
#include <QSlider>

class UDPServer : public QWidget
{
    Q_OBJECT

public:
    explicit UDPServer(QWidget *parent = nullptr);
    ~UDPServer();

signals:

private slots:
    void updateHeightLabel(int value);
    void readingDatagrams();
    void sendHeight();
    void signalClient();

private:
    QUdpSocket *udpSocketGet;
    QUdpSocket *udpSocketSend;
    QUdpSocket *udpSocket;
    QSlider *heightSlider;
    QLabel *statusLabel;
    QLabel *heightLabel;
    QTimer *timer;
    QTimer *timerSignal;
    QTime lastMessageTime;
    QTime twoSecondsAgo;
};

#endif // UDPSERVER_H
