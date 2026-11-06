#include "Utils.h"
#include <glm/common.hpp>

glm::vec3 Utils::Uint32ToVec3(uint32_t color)
{
    constexpr float inv = 1.0f / 255.0f;

    uint8_t r = static_cast<uint8_t>(color & 0xFF);
    uint8_t g = static_cast<uint8_t>((color >> 8) & 0xFF);
    uint8_t b = static_cast<uint8_t>((color >> 16) & 0xFF);
    // uint8_t a = static_cast<uint8_t>((color >> 24) & 0xFF);

    return glm::vec3(r * inv, g * inv, b * inv);
}

uint32_t Utils::Vec3ToUint32(glm::vec3 color)
{

    uint32_t r = static_cast<uint32_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
    uint32_t g = static_cast<uint32_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f);;
    uint32_t b = static_cast<uint32_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f + 0.5f);;
    // uint32_t a = static_cast<uint32_t>(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f + 0.5f);;

    // return (a << 24) | (b << 16) | (g << 8) | r;
    return (b << 16) | (g << 8) | r;
}