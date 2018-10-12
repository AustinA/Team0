#include "velodyneudpsocket.h"

VelodyneUDPSocket::VelodyneUDPSocket(QObject *parent) : QObject(parent)
{
   data = &Singleton<Team0::VelodyneDecodedData>::Instance();
}

VelodyneUDPSocket::~VelodyneUDPSocket()
{
    socket->close();
    delete socket;
}


void VelodyneUDPSocket::Start()
{
    socket = new QUdpSocket(this);
    if(!socket->bind(QHostAddress::AnyIPv4, 2368, QAbstractSocket::ShareAddress))
    {
        std::cout << "FAILED TO BIND" << std::endl;
        std::cout << socket->errorString().toStdString() << std::endl;
    }
    socket->joinMulticastGroup(QHostAddress("192.168.1.201"));

    connect(socket, SIGNAL(readyRead()), this, SLOT(Data()));

    std::cout << "ready" << std::endl;
}

void VelodyneUDPSocket::Stop()
{
    socket->abort();
}


void VelodyneUDPSocket::Data()
{
//    std::cout << "Data!" << std::endl;
//    socket->readAll();
    while(socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QByteArray byteArray = datagram.data();
        if(byteArray.size() != 1206)
        {
            return;
        }
        QDataStream ds(byteArray);
        Team0::VelodynePacket packet;
        for(int i = 0; i < Team0::ShortPacketSize; i ++)
        {
            ds >> packet.unformatted[i];
        }
//        for(uint16_t &unformatted : packet.unformatted)
//        {
//            ds >> unformatted;
//        }
        data->PushNewPacket(packet);

//        std::cout << data->_active_frame.points.size() << std::endl;
    }
}
