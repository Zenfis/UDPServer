#include "udpserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UDPServer server;
    server.show();

    return a.exec();
}
