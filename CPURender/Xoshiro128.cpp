#include "Xoshiro128.h"

Xoshiro128::Xoshiro128(uint32_t seed)
{
    for (int i = 0; i < 4; ++i)
    {
        seed += 0x9E3779B9u;
        uint32_t z = seed;
        z = (z ^ (z >> 16)) * 0x85EBCA6Bu;
        z = (z ^ (z >> 13)) * 0xC2B2AE35u;
        s[i] = z ^ (z >> 16);
    }
}
uint32_t Xoshiro128::Next()
{
    uint32_t result = Rotl(s[0] + s[3], 7) + s[0];

    uint32_t t = s[1] << 9;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = Rotl(s[3], 11);

    return result;
}

float Xoshiro128::RandomFloat()
{
    return (Next() >> 8) * (1.0f / 16777216.0f);
}

glm::vec2 Xoshiro128::RandomVec2()
{
    return glm::vec2(RandomFloat(), RandomFloat());
}

glm::vec3 Xoshiro128::RandomVec3()
{
    return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

uint32_t Xoshiro128::Rotl(uint32_t x, int k)
{
    return (x << k) | (x >> (32 - k));
}