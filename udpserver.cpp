#include "udpserver.h"
#include <QVBoxLayout>

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

UDPServer::UDPServer(QWidget *parent) :
   QWidget(parent)
{
   setWindowTitle("Сервер");
   setFixedSize(250, 100);
   setMinimumSize(250, 100);
   setMaximumSize(250, 100);

   statusLabel = new QLabel("Связь с клиентом: нет", this);
   heightLabel = new QLabel("Текущая высота: 0 м", this);
   heightSlider = new DoubleSlider(Qt::Horizontal, this);
   heightSlider->setRange(0, 9999000);

   QVBoxLayout *layout = new QVBoxLayout(this);

   layout->addWidget(statusLabel);
   layout->addWidget(heightLabel);
   layout->addWidget(heightSlider);
   setLayout(layout);

   udpSocket = new QUdpSocket(this);

   connect(heightSlider, &DoubleSlider::doubleValueChanged, this, &UDPServer::updateHeightLabel);

   timer = new QTimer(this);
   connect(timer, &QTimer::timeout, this, &UDPServer::sendHeight);
   timer->start(25000);

   connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readingDatagrams()));

   QSettings settings("settings.ini", QSettings::IniFormat);
   QString ipAddress = settings.value("settings/ipaddress").toString(); //TODO
   int port = settings.value("settings/port").toInt();

   if (!udpSocket->bind(QHostAddress::LocalHost, port)) {
       qDebug() << "Не удалось забиндить на адрес и порт";
   } else {
       qDebug() << "Забинжен на данный адрес и порт";
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
    msg1.height = value;
}

void UDPServer::sendHeight()
{
    //QByteArray datagram = heightLabel->text().toUtf8();
    //udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
    //qDebug() << "Отправлено на клиент:"
    //         << datagram.data();

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << msg1.header << msg1.height;

    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
    qDebug() << "Отправлена высота:" << msg1.height;
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
            udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
            curTime = QTime::currentTime();
            qDebug() << curTime.toString() << "- Pong";
            qDebug() << curTime.toString() << "- Ping";
        }
    }
}
