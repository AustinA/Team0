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

    Team0::VelodyneDecodedData *data = &Singleton<Team0::VelodyneDecodedData>::Instance();

    VelodyneSocketWrapper socket;
    socket.Start();

    auto start = clock::now();


//    socket.Start();
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        std::cout << "Total Frames collected: " << data->frames.size()
                  << "Active Frame: " << data->_active_frame.points.size()
                  << std::endl;
    }

//    socket.Stop();

//    socket.Stop();


    return a.exec();
}
