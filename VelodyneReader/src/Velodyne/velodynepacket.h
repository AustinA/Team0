#ifndef VELODYNEPACKET_H
#define VELODYNEPACKET_H

#include <array>
#include <cmath>

#include "singleton.h"

namespace Team0
{

static constexpr int NUM_ROT_ANGLES = 36001;
static constexpr int LASER_FIRINGS = 32;
static constexpr int LASER_COUNT = 64;
static constexpr int FIRINGS_PER_PACKET = 12;

enum class Block : unsigned short
{
    BLOCK_0_TO_31 = 0xeeff,
    BLOCK_32_TO_63 = 0xddff,
};

template<typename T>
constexpr auto HDLToRadians(T d)
{
    return (d * M_PI) / 180.0;
}


struct VelodyneHeader
{
    char stuff[42];
};


struct __attribute__((packed))  LazerReturn
{
    unsigned short distance;
    unsigned char intensity;
};

struct __attribute__((packed)) FiringBlock
{
    Block Flag;
    uint16_t azimuth;

    LazerReturn channelData1[LASER_FIRINGS];
};

enum class FactoryFlags : unsigned short
{
    STRONGEST_RETURN = 0x4D37,
    LAST_RETURN = 0x4D38,
    DUAL_RETURN = 0x4D39,
    HDL_32E = 0x4D21,
    VLP_16 = 0x4D22,
};

struct __attribute__((packed))  Tail
{
    uint32_t timestamp;
    FactoryFlags factory;
};

struct __attribute__((packed))  UnusedVelodynePacket
{
    VelodyneHeader header;
    FiringBlock lazer_firing_blocks[FIRINGS_PER_PACKET];
    Tail tail;
};

struct __attribute__((packed)) PackedVelodynePacket
{
    FiringBlock lazer_firing_blocks[FIRINGS_PER_PACKET];
    Tail tail;
};
static constexpr size_t ShortPacketSize = sizeof(PackedVelodynePacket);

union __attribute__((packed)) VelodynePacket
{
    PackedVelodynePacket formated;
    uint16_t unformatted[ShortPacketSize];
};

struct Correction
{
    double azimuth;
    double vertical;
    double distance;
    double verticalOffset;
    double horizontalOffset;
    double sinVert;
    double cosVert;
    double sinVertOffset;
    double cosVertOffset;
};



struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class SinLookup
{
public:
    SinLookup()
    {
        int i = 0;
        for(auto & s : data)
        {
            s = std::sin(HDLToRadians(i / 100.0));
            i++;
        }
    }

    template<int location>
    auto Get()
    {
        static_assert(location < NUM_ROT_ANGLES, "location out of bounds");

        return data[location];
    }


    auto Get(unsigned short location)
    {
        assert(location < NUM_ROT_ANGLES);
//        assert(location > 0);

        return data[location];
    }

private:
    std::array<double, NUM_ROT_ANGLES> data;
};

class CosLookup
{
public:

    CosLookup()
    {
        int i = 0;
        for(auto & s : data)
        {
            s = std::cos(HDLToRadians(i / 100.0));
            i++;
        }
    }


    template<int location>
    auto Get()
    {
        static_assert(location < NUM_ROT_ANGLES, "location out of bounds");

        return data[location];
    }

    auto Get(int location)
    {
        assert(location < NUM_ROT_ANGLES);

        return data[location];
    }

private:
    std::array<double, NUM_ROT_ANGLES> data;
};

using SinLookupTable = Singleton<SinLookup>;
using CosLookupTable = Singleton<CosLookup>;





}


#endif // VELODYNEPACKET_H
