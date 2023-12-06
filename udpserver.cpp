#include "udpserver.h"
#include <QFileInfo>

#pragma pack(push, 1)
struct Message1
{
    quint16 header = 0xABCD;
    quint16 height;
};
struct Message2
{
    quint16 header = 0x1234;
};
#pragma pack(pop)

QString settingsFilePath = "C:\\Users\\user01\\Documents\\projects\\UDPServer\\settings.ini";
QSettings settings("C:\\Users\\user01\\Documents\\projects\\UDPServer\\settings.ini", QSettings::IniFormat);
QString server_ip = settings.value("serverhost/server_ip").toString();
quint16 server_port = settings.value("serverhost/server_port").toUInt();
QString client_ip = settings.value("sendtoclient/client_ip").toString();
quint16 client_port = settings.value("sendtoclient/client_port").toUInt();

Message1 msg1;
Message2 msg2;

UDPServer::UDPServer(QWidget *parent) :
    QWidget(parent)
{
    //Интерфейс
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
    heightSlider = new QSlider(Qt::Horizontal, this);
    heightSlider->setRange(0, 9999);
    heightSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->setSpacing(2);
    layout->addWidget(statusLabel);
    layout->addWidget(heightLabel);
    layout->addWidget(heightSlider);
    setLayout(layout);
    }
    //Проверка на ошибки
    {
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
    }
    //Соединения
    {
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readingDatagrams()));
        connect(heightSlider, &QSlider::valueChanged, this, &UDPServer::updateHeightLabel);
        connect(timer, SIGNAL(timeout()), this, SLOT(sendHeight()));
        connect(timerSignal, SIGNAL(timeout()), this, SLOT(signalClient()));
        timerSignal->start(2000);
        timer->start(25);
    }
    qDebug() << "Сервер запущен!";
}

UDPServer::~UDPServer(){}

void UDPServer::signalClient()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << msg1.header;
    udpSocket->writeDatagram(datagram.constData(), datagram.size(), QHostAddress(client_ip), client_port);
}

void UDPServer::updateHeightLabel(int value)
{    
    heightLabel->setText(QString("Текущая высота: %1 м").arg(static_cast<int>(value)));
    msg1.height = static_cast<quint16>(value);
}

void UDPServer::sendHeight()
{
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

        lastMessageTime = QTime::currentTime();
        twoSecondsAgo = QTime::currentTime().addSecs(-2);

        if (datagram.size() == sizeof(Message2))
        {
            if (msg2.header == static_cast<int>(0x1234))
            {
                statusLabel->setText("Связь с клиентом: да");
            }
        }
        else if (lastMessageTime > twoSecondsAgo) { statusLabel->setText("Связь с клиентом: нет"); }
        else { statusLabel->setText("Связь с клиентом: нет"); }
    }
}
