#ifndef VELODYNESOCKETWRAPPER_H
#define VELODYNESOCKETWRAPPER_H

#include <QObject>
#include <QThread>

#include "velodyneudpsocket.h"

class VelodyneSocketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit VelodyneSocketWrapper(QObject *parent = nullptr) :
        QObject(parent)
    {
        socket.moveToThread(&myThread);
        myThread.start();
    }

    void Start()
    {
        socket.metaObject()->invokeMethod(&socket, "Start", Qt::QueuedConnection);
    }

    void Stop()
    {
        socket.metaObject()->invokeMethod(&socket, "Stop", Qt::QueuedConnection);
    }




private:
    QThread myThread;
    VelodyneUDPSocket socket;



};

#endif // VELODYNESOCKETWRAPPER_H
