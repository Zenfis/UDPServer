#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QWidget>
#include <QUdpSocket>
#include <QLabel>
#include <QTimer>
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

private:
    QUdpSocket *udpSocket;
    QLabel *statusLabel;
    QLabel *heightLabel;
    DoubleSlider *heightSlider;
    QTimer *timer;
};

#endif // UDPSERVER_H
