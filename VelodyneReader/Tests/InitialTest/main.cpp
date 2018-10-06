#include <QCoreApplication>
#include <QThread>

#include <iostream>

#include <chrono>
#include <thread>


#include "velodyneudpsocket.h"
#include "velodynepacket.h"
#include "velodynesocketwrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    std::cout << "packed packet size: " << sizeof(Team0::UnusedVelodynePacket) << std::endl;
//    std::cout << "packed packet size: " << sizeof(Team0::PackedVelodynePacket) << std::endl;

    std::cout << "Starting" << std::endl;

    using clock = std::chrono::steady_clock;

    VelodyneSocketWrapper socket;
    socket.Start();

    auto start = clock::now();


//    socket.Start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    socket.Stop();

//    socket.Stop();


    return a.exec();
}
