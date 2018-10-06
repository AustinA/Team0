#include <random>
#include <array>
#include <type_traits>

template <typename ReturnType, typename DistributionType>
class BasicRandomDevice
{
private:
    std::mt19937 engine;
    DistributionType distribution;

    bool seeded = false;
    int seed = 0;

    template <typename R, typename = std::enable_if<std::is_arithmetic<R>::value>>
    void SetupDistribution(R lower, R upper)
    {
        distribution = DistributionType(lower, upper);
    }

public:

    BasicRandomDevice()
    {
        std::random_device r;
        engine = std::mt19937(r());
        SetupDistribtuion(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
    }

    BasicRandomDevice(int s) : seed(s), seeded(true)
    {
        engine.seed(s);
        SetupDistribution(std::numeric_limits<ReturnType>::min(), std::numeric_limits<ReturnType>::max());
    }

    template <typename R, typename T,
                typename = std::enable_if<std::is_arithmetic<R>::value>,
                typename = std::enable_if<std::is_integral<T>::value>
                >
    BasicRandomDevice(R lower, R upper, T s = 0)
    {
        if(s > 0)
        {
            seeded = true;
            seed = s;
            engine.seed(s);
        }
        else
        {
            std::random_device r;
            engine = std::mt19937(r());
        }

        SetupDistribution(lower, upper);
    }

    ReturnType Get()
    {
        return distribution(engine);
    }

    int getSeed() const
    {
        return seeded? seed : -1;
    }

    void SeedEngine(int s)
    {
        engine.seed(s);
    }
};

template<typename T>
using UniformRealRandomDevice = BasicRandomDevice<T, std::uniform_real_distribution<T>>;

using UniformFloat32RandomDevice = UniformRealRandomDevice<float>;
using UniformFloat64RandomDevice = UniformRealRandomDevice<double>;

template<typename T>
using UniformIntRandomDevice = BasicRandomDevice<T, std::uniform_int_distribution<T>>;

using UniformUInt8RandomDevice = UniformIntRandomDevice<uint8_t>;
using UniformUInt16RandomDevice = UniformIntRandomDevice<uint16_t>;
using UniformUInt32RandomDevice = UniformIntRandomDevice<uint32_t>;
using UniformUInt64RandomDevice = UniformIntRandomDevice<uint64_t>;
