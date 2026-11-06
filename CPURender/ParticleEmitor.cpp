#include "ParticleEmitor.h"

ParticleEmitor ParticleEmitor::CreateFireCoreEmitor(glm::vec3 position)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -0.15f, 0.f, -0.15f };
	pe.m_offsetPositionMax = { 0.15f, 0.05f, 0.15f };

	pe.m_directionsMin = { -0.1f, 1.f, -0.1f };
	pe.m_directionsMax = { 0.1f, 1.f, 0.1f };
	pe.m_speedMin = 1.5f;
	pe.m_speedMax = 2.5f;

	pe.m_sizeMin = { 0.12f, 0.12f };
	pe.m_sizeMax = { 0.25f, 0.25f };

	pe.m_lifeMin = 0.4f;
	pe.m_lifeMax = 0.8f;

	pe.m_friction = 0.6f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 1.0f, 0.95f, 0.5f };  
	pe.m_colorEnd = { 1.0f, 0.45f, 0.05f }; 

	pe.m_emissionRate = 60.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateFireFlameEmitor(glm::vec3 position)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -0.35f, 0.f, -0.35f };
	pe.m_offsetPositionMax = { 0.35f, 0.1f, 0.35f };

	pe.m_directionsMin = { -0.2f, 1.f, -0.2f };
	pe.m_directionsMax = { 0.2f, 1.f, 0.2f };
	pe.m_speedMin = 1.2f;
	pe.m_speedMax = 2.8f;

	pe.m_sizeMin = { 0.15f, 0.15f };
	pe.m_sizeMax = { 0.35f, 0.35f };

	pe.m_lifeMin = 0.6f;
	pe.m_lifeMax = 1.3f;

	pe.m_friction = 0.5f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 1.0f, 0.6f, 0.1f };  
	pe.m_colorEnd = { 0.6f, 0.05f, 0.0f }; 

	pe.m_emissionRate = 70.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateSmokeEmitor(glm::vec3 position, glm::vec3 wind)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -0.3f, 1.2f, -0.3f };
	pe.m_offsetPositionMax = { 0.3f, 1.5f, 0.3f };

	pe.m_directionsMin = glm::vec3(-0.15f, 1.f, -0.15f) + wind;
	pe.m_directionsMax = glm::vec3(0.15f, 1.f, 0.15f) + wind;
	pe.m_speedMin = 0.8f;
	pe.m_speedMax = 1.5f;

	pe.m_sizeMin = { 0.3f, 0.3f };
	pe.m_sizeMax = { 0.7f, 0.7f };

	pe.m_lifeMin = 2.0f;
	pe.m_lifeMax = 3.5f;

	pe.m_friction = 0.3f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 0.30f, 0.30f, 0.30f };
	pe.m_colorEnd = { 0.08f, 0.08f, 0.08f };

	pe.m_emissionRate = 8.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateRainEmitor(glm::vec3 center, float areaSize, float height, float density)
{
	float speed = 25.f;

	ParticleEmitor pe;
	pe.m_emitorPosition = center + glm::vec3(0.f, height, 0.f);
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -areaSize * 0.5f, 0.f, -areaSize * 0.5f };
	pe.m_offsetPositionMax = { areaSize * 0.5f, 0.f, areaSize * 0.5f };

	pe.m_directionsMin = { -0.05f, -1.f, -0.05f };
	pe.m_directionsMax = { 0.05f, -1.f, 0.05f };
	pe.m_speedMin = speed;
	pe.m_speedMax = speed;

	pe.m_sizeMin = { 0.02f, 0.35f };
	pe.m_sizeMax = { 0.04f, 0.60f };

	pe.m_lifeMin = height / speed;
	pe.m_lifeMax = height / speed;

	pe.m_friction = 0.f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 0.65f, 0.75f, 0.95f };
	pe.m_colorEnd = { 0.50f, 0.60f, 0.85f };

	pe.m_emissionRate = density;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateSparkEmitor(glm::vec3 position)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -0.05f, 0.f, -0.05f };
	pe.m_offsetPositionMax = { 0.05f, 0.05f, 0.05f };

	pe.m_directionsMin = { -0.6f, 0.4f, -0.6f };
	pe.m_directionsMax = { 0.6f, 1.2f, 0.6f };
	pe.m_speedMin = 3.f;
	pe.m_speedMax = 8.f;

	pe.m_sizeMin = { 0.03f, 0.03f };
	pe.m_sizeMax = { 0.06f, 0.06f };

	pe.m_lifeMin = 0.4f;
	pe.m_lifeMax = 1.0f;

	pe.m_friction = 0.3f;
	pe.m_isGravityAffected = true;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 1.0f, 0.95f, 0.6f };
	pe.m_colorEnd = { 1.0f, 0.3f, 0.0f };

	pe.m_emissionRate = 40.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateEmbersEmitor(glm::vec3 position)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -0.4f, 0.2f, -0.4f };
	pe.m_offsetPositionMax = { 0.4f, 0.5f, 0.4f };

	pe.m_directionsMin = { -0.3f, 0.6f, -0.3f };
	pe.m_directionsMax = { 0.5f, 1.0f, 0.3f };
	pe.m_speedMin = 0.5f;
	pe.m_speedMax = 1.5f;

	pe.m_sizeMin = { 0.02f, 0.02f };
	pe.m_sizeMax = { 0.05f, 0.05f };

	pe.m_lifeMin = 2.f;
	pe.m_lifeMax = 4.f;

	pe.m_friction = 0.1f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 1.0f, 0.6f, 0.1f };
	pe.m_colorEnd = { 0.4f, 0.05f, 0.0f };

	pe.m_emissionRate = 6.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateSnowEmitor(glm::vec3 position)
{
	float areaSize = 40.f;
	float height = 20.f;
	float speed = 1.5f;

	ParticleEmitor pe;
	pe.m_emitorPosition = position + glm::vec3(0.f, height, 0.f);
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -areaSize * 0.5f, 0.f, -areaSize * 0.5f };
	pe.m_offsetPositionMax = { areaSize * 0.5f, 0.f, areaSize * 0.5f };

	pe.m_directionsMin = { -0.3f, -1.f, -0.3f };
	pe.m_directionsMax = { 0.3f, -0.6f, 0.3f };
	pe.m_speedMin = speed * 0.7f;
	pe.m_speedMax = speed * 1.3f;

	pe.m_sizeMin = { 0.04f, 0.04f };
	pe.m_sizeMax = { 0.09f, 0.09f };

	pe.m_lifeMin = height / speed * 1.2f;
	pe.m_lifeMax = height / speed * 1.6f;

	pe.m_friction = 0.f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 1.f, 1.f, 1.f };
	pe.m_colorEnd = { 0.85f, 0.9f, 1.f };

	pe.m_emissionRate = 400.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateFirefliesEmitor(glm::vec3 position, float areaSize)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -areaSize * 0.5f, 0.f, -areaSize * 0.5f };
	pe.m_offsetPositionMax = { areaSize * 0.5f, 2.f, areaSize * 0.5f };

	pe.m_directionsMin = { -1.f, -0.5f, -1.f };
	pe.m_directionsMax = { 1.f, 0.5f, 1.f };
	pe.m_speedMin = 0.1f;
	pe.m_speedMax = 0.4f;

	pe.m_sizeMin = { 0.05f, 0.05f };
	pe.m_sizeMax = { 0.08f, 0.08f };

	pe.m_lifeMin = 4.f;
	pe.m_lifeMax = 8.f;

	pe.m_friction = 0.f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 0.9f, 1.0f, 0.3f };
	pe.m_colorEnd = { 0.2f, 0.6f, 0.1f };

	pe.m_emissionRate = 3.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateDustEmitor(glm::vec3 position)
{
	float areaSize = 10.f;

	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -areaSize * 0.5f, 0.f, -areaSize * 0.5f };
	pe.m_offsetPositionMax = { areaSize * 0.5f, 3.f, areaSize * 0.5f };

	pe.m_directionsMin = { -1.f, -0.2f, -1.f };
	pe.m_directionsMax = { 1.f, 0.2f, 1.f };
	pe.m_speedMin = 0.05f;
	pe.m_speedMax = 0.2f;

	pe.m_sizeMin = { 0.02f, 0.02f };
	pe.m_sizeMax = { 0.04f, 0.04f };

	pe.m_lifeMin = 6.f;
	pe.m_lifeMax = 10.f;

	pe.m_friction = 0.f;
	pe.m_isGravityAffected = false;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 0.75f, 0.68f, 0.55f };
	pe.m_colorEnd = { 0.55f, 0.50f, 0.40f };

	pe.m_emissionRate = 5.f;
	return pe;
}

ParticleEmitor ParticleEmitor::CreateFountainEmitor(glm::vec3 position, float areaSize)
{
	ParticleEmitor pe;
	pe.m_emitorPosition = position;
	pe.m_isEmitorEnabled = true;

	pe.m_offsetPositionMin = { -areaSize * 0.5f, 0.f, -areaSize * 0.5f };
	pe.m_offsetPositionMax = { areaSize * 0.5f, 0.f, areaSize * 0.5f };

	pe.m_directionsMin = { -0.15f, 1.f, -0.15f };
	pe.m_directionsMax = { 0.15f, 1.f, 0.15f };
	pe.m_speedMin = 6.f;
	pe.m_speedMax = 8.f;

	pe.m_sizeMin = { 0.05f, 0.05f };
	pe.m_sizeMax = { 0.10f, 0.10f };

	pe.m_lifeMin = 1.4f;
	pe.m_lifeMax = 1.8f;

	pe.m_friction = 0.05f;
	pe.m_isGravityAffected = true;

	pe.m_textureIndex = static_cast<uint32_t>(-1);
	pe.m_colorBegin = { 0.7f, 0.85f, 1.0f };
	pe.m_colorEnd = { 0.2f, 0.4f, 0.8f };

	pe.m_emissionRate = 150.f;
	return pe;
}