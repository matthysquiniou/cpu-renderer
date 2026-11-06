#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class Xoshiro128
{
public:
    Xoshiro128(uint32_t seed = 123456789u);
    uint32_t Next();
    float RandomFloat();
    glm::vec2 RandomVec2();
    glm::vec3 RandomVec3();

private:
    static uint32_t Rotl(uint32_t x, int k);

private:
    uint32_t s[4];
};

