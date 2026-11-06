#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Particles
{
	std::vector<glm::vec3>* m_positions;
	std::vector<glm::vec3>* m_velocities;
	std::vector<glm::vec2>* m_sizes; 
	std::vector<glm::vec2>* m_lifes; // x = current y = max
	std::vector<uint32_t>* m_textureIndex; 
	std::vector<std::pair<glm::vec3,glm::vec3>>* m_colors; // x colorBegin y colorEnd
	std::vector<float>* m_frictions;
	std::vector<bool>* m_isGravityAffected;
};