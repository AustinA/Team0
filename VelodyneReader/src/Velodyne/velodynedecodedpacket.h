#ifndef VELODYNEDECODEDPACKET_H
#define VELODYNEDECODEDPACKET_H

#include <deque>
#include <array>
#include <vector>

#include <iostream>

#include "velodynepacket.h"
#include "singleton.h"

namespace Team0
{


struct VelodyneDecodedLazerPoint
{
    double x;
    double y;
    double z;
    double distance;
    uint8_t intensity;
    uint8_t lazer_id;
    int16_t azimuth;
    uint32_t timestamp;

    void Clear()
    {

    }

    VelodyneDecodedLazerPoint() = default;

    VelodyneDecodedLazerPoint(
            const VelodyneDecodedLazerPoint& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        distance = other.distance;
        intensity = other.intensity;
        lazer_id = other.lazer_id;
        azimuth = other.azimuth;
        timestamp = other.timestamp;
    }

    VelodyneDecodedLazerPoint& operator=(
            const VelodyneDecodedLazerPoint& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        distance = other.distance;
        intensity = other.intensity;
        lazer_id = other.lazer_id;
        azimuth = other.azimuth;
        timestamp = other.timestamp;
        return *this;
    }

    VelodyneDecodedLazerPoint(
            VelodyneDecodedLazerPoint&& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        distance = other.distance;
        intensity = other.intensity;
        lazer_id = other.lazer_id;
        azimuth = other.azimuth;
        timestamp = other.timestamp;
        other.Clear();
    }

    VelodyneDecodedLazerPoint& operator=(
            VelodyneDecodedLazerPoint&& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        distance = other.distance;
        intensity = other.intensity;
        lazer_id = other.lazer_id;
        azimuth = other.azimuth;
        timestamp = other.timestamp;
        other.Clear();
        return *this;
    }

};

struct VelodyneDecodedLazerFrame
{
    void Clear()
    {
        points.clear();
    }

    void Append(const VelodyneDecodedLazerPoint &f)
    {
        points.push_back(f);
    }

    VelodyneDecodedLazerFrame() = default;

    VelodyneDecodedLazerFrame(const VelodyneDecodedLazerFrame &other)
    {
        points.resize(other.points.size());
        for(const auto & p : other.points)
        {
            points.push_back(p);
        }
    }

    VelodyneDecodedLazerFrame& operator=(
            const VelodyneDecodedLazerFrame& other)
    {
        points.resize(other.points.size());
        for(const auto & p : other.points)
        {
            points.push_back(p);
        }
        return *this;
    }

    VelodyneDecodedLazerFrame(VelodyneDecodedLazerFrame &&other)
    {
        points.resize(other.points.size());
        for(const auto & p : other.points)
        {
            points.push_back(p);
        }
        other.Clear();
    }

    VelodyneDecodedLazerFrame& operator=(
            VelodyneDecodedLazerFrame && other)
    {
        points.resize(other.points.size());
        for(const auto & p : other.points)
        {
            points.push_back(p);
        }
        other.Clear();
        return *this;
    }


    static constexpr int max_points = 1024;
    std::deque<VelodyneDecodedLazerPoint> points;
//    std::array<VelodyneDecodedLazerPoint, max_points> points;
//    int max = 0;
};

class VelodyneDecodedData
{
public:
    VelodyneDecodedLazerFrame _active_frame;
    std::deque<VelodyneDecodedLazerFrame> frames;

    void PushNewPacket(const VelodynePacket &packet)
    {
        for(const FiringBlock & firing : packet.formated.lazer_firing_blocks)
        {
            const uint16_t offset = (firing.Flag == Block::BLOCK_0_TO_31) ? 0 : 32;

            if(firing.azimuth < _last_azimuth)
            {
                std::cout << "Test: " << _active_frame.points.size() << std::endl;
                frames.push_back(_active_frame);
                _active_frame.Clear();
            }

            _last_azimuth = firing.azimuth;

            int i = 0;
            for(const LazerReturn & lazer : firing.channelData1)
            {

                VelodyneDecodedLazerPoint point;
                uint8_t lazerID = (i++) + offset;
                Correction corr;
                try
                {
                    corr = lazerCorrections.at(lazerID);
                }
                catch(std::exception &e)
                {
                    std::cout << "Out of bounds lazer id: " << e.what()
                              << " \nLazerID: " << lazerID << std::endl;
                }

                double sinAzimuth = CalcSin(lazerID, firing.azimuth);
                double cosAzimuth = CalcCos(lazerID, firing.azimuth);

                double distanceM = lazer.distance * 0.002 + corr.distance;
                double xyDistance = distanceM * corr.cosVert - corr.sinVertOffset;

                uint8_t intensity = lazer.intensity;

                point.distance = distanceM * corr.cosVert - corr.sinVertOffset;;

                point.azimuth = firing.azimuth;
                point.intensity = lazer.intensity;
                point.lazer_id = lazerID;
                point.timestamp = packet.formated.tail.timestamp;
                point.x =  (xyDistance * sinAzimuth - corr.horizontalOffset * cosAzimuth);
                point.y = (xyDistance * cosAzimuth + corr.horizontalOffset * sinAzimuth);
                point.z = (distanceM * corr.sinVert + corr.cosVertOffset);

                _active_frame.points.push_back(point);
            }
        }
    }

private:

int16_t _last_azimuth = 0;
std::array<Correction, LASER_COUNT>lazerCorrections;


double CalcSin(int id, int azimuth)
{
    if(lazerCorrections[id].azimuth <= 0)
    {
        return SinLookupTable::Instance().Get(azimuth);
    }

    return std::sin(HDLToRadians(azimuth) - lazerCorrections[id].azimuth);
}

double CalcCos(int id, int azimuth)
{
    if(lazerCorrections[id].azimuth <= 0)
    {
        return CosLookupTable::Instance().Get(azimuth);
    }

    return std::cos(HDLToRadians(azimuth) - lazerCorrections[id].azimuth);
}

};

struct VelodyneDataStore
{
    std::vector<VelodyneDecodedData> data;

//    void
};


using GlobalDecodedData = Singleton<VelodyneDecodedData>;


}

#endif // VELODYNEDECODEDPACKET_H
