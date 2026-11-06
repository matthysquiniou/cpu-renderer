#pragma once
#include <glm/mat4x4.hpp>
#include "Light.h"
#include "Particles.h"

struct ConstantBuffer
{
	glm::mat4x4 m_view;
	glm::mat4x4 m_proj;
	glm::mat4x4 m_viewProj;
	glm::mat4x4 m_model;
	glm::mat4x4 m_InvTransposeModel = glm::mat4x4(1);
	glm::vec3 m_cameraPos;
	float m_totalTime;
	float m_deltaTime;
	LightsData* m_lightsData;
	Particles* m_particles = nullptr;
	uint32_t m_particleAlive;
	std::vector<uint32_t> m_TexturesIndices;
	// ajouter d'autres trucs au besoin
};