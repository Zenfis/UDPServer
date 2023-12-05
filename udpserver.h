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
#include "doubleslider.h"

class UDPServer : public QWidget
{
    Q_OBJECT

public:
    explicit UDPServer(QWidget *parent = nullptr);
    ~UDPServer();

signals:

private slots:
    void updateHeightLabel(double value);
    void readingDatagrams();
    void sendHeight();
    void signalClient();

private:
    QUdpSocket *udpSocketGet;
    QUdpSocket *udpSocketSend;
    QUdpSocket *udpSocket;
    DoubleSlider *heightSlider;
    QLabel *statusLabel;
    QLabel *heightLabel;
    QTimer *timer;
    QTimer *timerSignal;
    QTime curTime;
};

#endif // UDPSERVER_H
