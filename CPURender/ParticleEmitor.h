#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct ParticleEmitor
{
	glm::vec3 m_emitorPosition;
	bool m_isEmitorEnabled;

	glm::vec3 m_offsetPositionMin;
	glm::vec3 m_offsetPositionMax;

	glm::vec3 m_directionsMin;
	glm::vec3 m_directionsMax;
	float m_speedMin;
	float m_speedMax;

	glm::vec2 m_sizeMin;
	glm::vec2 m_sizeMax;

	float m_lifeMin;
	float m_lifeMax;

	float m_friction;
	bool m_isGravityAffected;

	uint32_t m_textureIndex = -1; //let m_textureIndex at max value to use color gradiant 
	glm::vec3 m_colorBegin;
	glm::vec3 m_colorEnd;

	float m_emissionRate;
	float m_spawnAccumulator = 0.f;

	static ParticleEmitor CreateFireCoreEmitor(glm::vec3 position);
	static ParticleEmitor CreateFireFlameEmitor(glm::vec3 position);
	static ParticleEmitor CreateSmokeEmitor(glm::vec3 position, glm::vec3 wind = { 0.2f, 0.f, 0.f });
	static ParticleEmitor CreateRainEmitor(glm::vec3 center, float areaSize = 40.f, float height = 25.f, float density = 1500.f);
	static ParticleEmitor CreateSparkEmitor(glm::vec3 position);
	static ParticleEmitor CreateEmbersEmitor(glm::vec3 position);
	static ParticleEmitor CreateSnowEmitor(glm::vec3 position);
	static ParticleEmitor CreateFirefliesEmitor(glm::vec3 position, float areaSize = 6.f);
	static ParticleEmitor CreateDustEmitor(glm::vec3 position);
	static ParticleEmitor CreateFountainEmitor(glm::vec3 position, float areaSize = 10.f);
};