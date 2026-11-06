#pragma once
#include <glm/vec3.hpp>

namespace Utils
{
	glm::vec3 Uint32ToVec3(uint32_t color);
	uint32_t Vec3ToUint32(glm::vec3 color);
}