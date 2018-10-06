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

};

struct VelodyneDecodedLazerFrame
{
    void Clear()
    {
        points.clear();
//        for(auto & p : points)
//        {
//            p.Clear();
//        }
//        max = 0;
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


//    void Append(const VelodyneDecodedLazerPoint &f)
//    {
//        if(max < max_points)
//        {
//            points[max] = f;
//            max++;
//        }
//    }

    static constexpr int max_points = 1024;
    std::vector<VelodyneDecodedLazerPoint> points;
//    std::array<VelodyneDecodedLazerPoint, max_points> points;
//    int max = 0;
};

class VelodyneDecodedData
{
public:
    VelodyneDecodedLazerFrame _active_frame;
    std::vector<VelodyneDecodedLazerFrame> frames;

    VelodyneDecodedData() = default;

    void PushNewPacket(const VelodynePacket &packet)
    {
        for(const FiringBlock & firing : packet.formated.lazer_firing_blocks)
        {
            const uint16_t offset = (firing.Flag == Block::BLOCK_0_TO_31) ? 0 : 32;

            if(firing.azimuth < _last_azimuth)
            {
                std::cout << "Test: " << _active_frame.points.size() << std::endl;
                frames.push_back(_active_frame);
                std::cout << "Test: " << _active_frame.points.size() << std::endl;
                _active_frame.Clear();
            }

            _last_azimuth = firing.azimuth;

            int i = 0;
            for(const LazerReturn & lazer : firing.channelData1)
            {
                uint8_t lazerID = (i++) + offset;
                const Correction &corr = lazerCorrections[lazerID];

                double sinAzimuth = CalcSin(lazerID, firing.azimuth);
                double cosAzimuth = CalcCos(lazerID, firing.azimuth);

                double distanceM = lazer.distance * 0.002 + corr.distance;
                double xyDistance = distanceM * corr.cosVert - corr.sinVertOffset;

                double x =  (xyDistance * sinAzimuth - corr.horizontalOffset * cosAzimuth);
                double y = (xyDistance * cosAzimuth + corr.horizontalOffset * sinAzimuth);
                double z = (distanceM * corr.sinVert + corr.cosVertOffset);

                uint8_t intensity = lazer.intensity;
                _active_frame.Append( VelodyneDecodedLazerPoint{
                                          x, y, z, distanceM, intensity,
                                          lazerID, firing.azimuth,
                                          packet.formated.tail.timestamp
                                      });

            }

        }
    }

private:

int16_t _last_azimuth;
Correction lazerCorrections[LASER_COUNT];


double CalcSin(int id, int azimuth)
{
    if(lazerCorrections[id].azimuth == 0)
    {
        return SinLookupTable::Instance().Get(azimuth);
    }

    return std::sin(HDLToRadians(azimuth) - lazerCorrections[id].azimuth);
}

double CalcCos(int id, int azimuth)
{
    if(lazerCorrections[id].azimuth == 0)
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
