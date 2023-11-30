#include "udpserver.h"

#pragma pack(push, 1)
struct Message1
{
    quint16 header = 0xABCD;
    quint16 height;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Message2
{
    quint16 header = 0x1234;
};
#pragma pack(pop)

Message1 msg1;
Message2 msg2;

QString local_ip = "127.0.0.1";
quint16 local_port = 9999;
QHostAddress send_to_ip = QHostAddress::LocalHost;
quint16 send_to_port = 1111;

UDPServer::UDPServer(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    timer = new QTimer(this);
    udpSocket = new QUdpSocket(this);

    setWindowTitle("Сервер");
    setFixedSize(250, 100);
    setMinimumSize(250, 100);
    setMaximumSize(250, 100);
    statusLabel = new QLabel("Связь с клиентом: нет", this);
    heightLabel = new QLabel("Текущая высота: 0 м", this);
    heightSlider = new DoubleSlider(Qt::Horizontal, this);
    heightSlider->setRange(0, 9999000);
    layout->addWidget(statusLabel);
    layout->addWidget(heightLabel);
    layout->addWidget(heightSlider);
    setLayout(layout);

     /*QSettings settings("settings.ini", QSettings::IniFormat);
    QString ip = settings.value("network/server_address", "default_ip").toString();
    quint16 port = settings.value("network/server_port", 0).toUInt();*/

    /*foreach(const QHostAddress &laddr, QNetworkInterface::allAddresses())
    {
        qDebug() << "Found IP:" << laddr.toString();
    }*/

    //if (udpSocket->bind(QHostAddress(ip), port))
    if (udpSocket->bind(QHostAddress(local_ip), local_port))
    {
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readingDatagrams()));
        connect(heightSlider, &DoubleSlider::doubleValueChanged, this, &UDPServer::updateHeightLabel);
        connect(timer, &QTimer::timeout, this, &UDPServer::sendHeight);
        timer->start(25000);
    }
    else
    {
       qDebug() << "Error";
    }
}

UDPServer::~UDPServer(){}

void UDPServer::updateHeightLabel(double value)
{
    if (value == std::floor(value))
    {
       heightLabel->setText(QString("Текущая высота: %1 м").arg(static_cast<int>(value)));
    }
    else
    {
       heightLabel->setText(QString("Текущая высота: %1 м").arg(value, 0, 'f', 2));
    }
    msg1.height = static_cast<quint16>(value); //fix
}

void UDPServer::sendHeight()
{
    /*QByteArray datagram = heightLabel->text().toUtf8();
    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
    qDebug() << "Отправлено на клиент:"
             << datagram.data();*/

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << msg1.header << msg1.height;

    udpSocket->writeDatagram(datagram.constData(), datagram.size(), send_to_ip, send_to_port);
    qDebug() << "Server sent height value:" << msg1.height;
}

void UDPServer::readingDatagrams()
{
    QHostAddress sender;
    quint16 senderPort;

    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QDataStream in(&datagram, QIODevice::ReadOnly);

        in >> msg2.header;

        if (msg2.header == 4660)
        {
            statusLabel->setText("Связь с клиентом: да");

            QDataStream out(&datagram, QIODevice::WriteOnly);
            out << msg1.header << msg1.height;
            udpSocket->writeDatagram(datagram.constData(), datagram.size(), send_to_ip, send_to_port);
            curTime = QTime::currentTime();
            qDebug() << curTime.toString() << "- Pong";
            qDebug() << curTime.toString() << "- Ping";
        }
    }
}
