#include "udpserver.h"
#include <QVBoxLayout>

#pragma pack(push, 1)
struct Message1
{
    quint16 header = 0xABCD;
    quint16 height;
};
#pragma pack(pop)

quint16 dHeight;

UDPServer::UDPServer(QWidget *parent) :
   QWidget(parent)
{
   setWindowTitle("Сервер");
   setFixedSize(250, 100);
   setMinimumSize(250, 100);
   setMaximumSize(250, 100);

   statusLabel = new QLabel("Связь с клиентом: НЕТ", this);
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

   connect(heightSlider, &DoubleSlider::doubleValueChanged, this, &UDPServer::sendHeight);

   connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readingDatagrams()));

   if (!udpSocket->bind(QHostAddress::LocalHost, 9999)) {
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
}

void UDPServer::sendHeight()
{
    QByteArray datagram = heightLabel->text().toUtf8();
    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
    qDebug() << "Отправлено на клиент: "
             << datagram.data();

    /*Message1 message;
    message.height = ;

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << message.header << message.height;

    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress::LocalHost, 1111);
    qDebug() << message.header << message.height;*/
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
       statusLabel->setText(QString(datagram));
       qDebug() << datagram.data() << " | IP: " + sender.toString() << "Port: " + QString("%1").arg(senderPort);
    }
}
