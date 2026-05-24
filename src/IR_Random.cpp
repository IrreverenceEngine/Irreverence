#include <IR_Random.hpp>

#include <random>

namespace IR::Random {
    static UInt64 s_Seed[2];
    static UInt64 splitmix64_seed = 0x123456789ABCDEF0ULL;

    UInt64 Rotl(UInt64 x, UInt32 k) {
        return (x << k) | (x >> (64 - k));
    }

    UInt64 RandomUInt64() {
        const UInt64 s0 = s_Seed[0];
        UInt64 s1 = s_Seed[1];
        const UInt64 result = s0 + s1;

        s1 ^= s0;
        s_Seed[0] = Rotl(s0, 55) ^ s1 ^ (s1 << 14);
        s_Seed[1] = Rotl(s1, 36);

        return result;
    }

    UInt64 SplitMix64() {
        UInt64 z = (splitmix64_seed += 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }

    void Seed(UInt64 seed)
    {
        splitmix64_seed = seed;
        s_Seed[0] = SplitMix64();
        s_Seed[1] = SplitMix64();
    }

    void SeedRandom()
    {
        std::random_device rd;
        Seed(((UInt64)rd() << 32) | rd());
    }

    Int64 Int(Int64 min, Int64 max) IRX_RETURN(min + (RandomUInt64() % (max - min + 1)))
    Float64 Float(Float64 min, Float64 max) IRX_RETURN(
        min + (RandomUInt64() >> 11) * (1.0 / (1ULL << 53)) * (max - min);
    )

}
