#include "udpserver.h"
#include <QFileInfo>

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

QString settingsFilePath = "C:\\Users\\user01\\Documents\\projects\\UDPServer\\settings.ini";
QSettings settings("C:\\Users\\user01\\Documents\\projects\\UDPServer\\settings.ini", QSettings::IniFormat);
QString server_ip = settings.value("serverhost/server_ip").toString();
quint16 server_port = settings.value("serverhost/server_port").toUInt();
QString client_ip = settings.value("sendtoclient/client_ip").toString();
quint16 client_port = settings.value("sendtoclient/client_port").toUInt();

UDPServer::UDPServer(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    timer = new QTimer(this);
    timerSignal = new QTimer(this);
    udpSocket = new QUdpSocket(this);

    setWindowTitle("Сервер");
    setFixedSize(220, 80);
    statusLabel = new QLabel("Связь с клиентом: нет", this);
    heightLabel = new QLabel("Текущая высота: 0 м", this);
    heightLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    heightSlider = new DoubleSlider(Qt::Horizontal, this);
    heightSlider->setRange(0, 9999000);
    heightSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->setSpacing(2);
    layout->addWidget(statusLabel);
    layout->addWidget(heightLabel);
    layout->addWidget(heightSlider);
    setLayout(layout);

    if (!QFileInfo::exists(settingsFilePath)) { qDebug() << "Файл настроек не найден"; return; }
    if (server_ip.isEmpty()) { qDebug() << "server_ip не указан в settings.ini"; return; }
    if (server_port == 0) { qDebug() << "server_port не указан в settings.ini"; return; }
    if (client_ip.isEmpty()) { qDebug() << "client_ip не указан в settings.ini"; return; }
    if (client_port == 0) { qDebug() << "client_port не указан в settings.ini"; return; }

    QHostAddress serverAddress;
    bool isValid = serverAddress.setAddress(server_ip);

    if (!isValid) { qDebug() << "Неверный IP адрес"; return; }
    if (udpSocket->state() == QAbstractSocket::BoundState) { qDebug() << "Сокет уже привязан"; return; }

    bool socketBinded = udpSocket->bind(serverAddress, server_port);

    if (!socketBinded) { qDebug() << "Ошибка привязки сокета: " << udpSocket->errorString(); return; }

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readingDatagrams()));
    connect(heightSlider, &DoubleSlider::doubleValueChanged, this, &UDPServer::updateHeightLabel);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendHeight()));
    connect(timerSignal, SIGNAL(timeout()), this, SLOT(signalClient()));
    timer->start(23000);
    timerSignal->start(2000);

    qDebug() << "Сервер запущен!";
}

UDPServer::~UDPServer(){}

void UDPServer::signalClient()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << msg1.header << msg1.height;
    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress(client_ip), client_port);
    curTime = QTime::currentTime();
    qDebug() << curTime.toString() << "- Ping";
}

void UDPServer::updateHeightLabel(double value)
{
    if (value == std::floor(value)) { heightLabel->setText(QString("Текущая высота: %1 м").arg(static_cast<int>(value))); }
    else { heightLabel->setText(QString("Текущая высота: %1 м").arg(value, 0, 'f', 2)); }
    msg1.height = static_cast<quint16>(value);
}

void UDPServer::sendHeight()
{
    QByteArray dgLabel = heightLabel->text().toUtf8();
    udpSocket->writeDatagram(dgLabel.constData(), dgLabel.size(), QHostAddress(client_ip), client_port);

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << msg1.header << msg1.height;

    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress(client_ip), client_port);
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

            curTime = QTime::currentTime();
            qDebug() << curTime.toString() << "- Pong";
        }
    }
}
