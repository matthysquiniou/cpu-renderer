#pragma once
#include <glm/vec3.hpp>
#include <vector>

struct AmbiantLight
{
	glm::vec3 m_color = { 1.f, 1.f, 1.f };
	float m_intensity = 0.2f;
};

struct DirectionalLight
{
	glm::vec3 m_color = { 1.f, 1.f ,1.f };
	glm::vec3 m_dir = { 0,-1,0 };
	float m_intensity = 1.f;
};

struct PointLight
{
	glm::vec3 m_color = { 1.f, 1.f ,1.f };
	glm::vec3 m_position = { 0.f, 0.f ,0.f };
	float m_intensity = 1.f;
	float m_range = 10.f;
};

struct SpotLight
{
	glm::vec3 m_color = { 1.f, 1.f ,1.f };
	glm::vec3 m_position = { 0.f, 0.f ,0.f };
	glm::vec3 m_dir = { 0,-1,0 };
	float m_intensity = 1.f;
	float m_beginAngleRad;
	float m_endAngleRad;
	float m_range = 10.f;
	float m_invRange;
};

struct LightsData
{
	AmbiantLight* m_ambiantLight;
	std::vector<DirectionalLight>* m_directionalLights;
	std::vector<PointLight>* m_pointLights;
	std::vector<SpotLight>* m_spotLights;
};
