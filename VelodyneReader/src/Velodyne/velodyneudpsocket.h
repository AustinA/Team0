#ifndef VELODYNEUDPSOCKET_H
#define VELODYNEUDPSOCKET_H

#include <QObject>
#include <QUdpSocket>
#include <QDataStream>
#include <QNetworkDatagram>
#include <QThread>
#include <QNetworkInterface>

#include <thread>

#include <iostream>

#include "velodynepacket.h"
#include "velodynedecodedpacket.h"
#include "singleton.h"

class VelodyneUDPSocket : public QObject
{
    Q_OBJECT
public:
    explicit VelodyneUDPSocket(QObject *parent = nullptr);
    ~VelodyneUDPSocket();


signals:

public slots:
    void Data();
    void Start();
    void Stop();


private:
    QUdpSocket *socket;
    Team0::VelodyneDecodedData *data;
//    std::thread socketThread;
//    bool running = false;
//    QDataStream datastream;


};

#endif // VELODYNEUDPSOCKET_H
