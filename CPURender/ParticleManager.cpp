#include "ParticleManager.h"
#include <iostream>

ParticleManager::ParticleManager(uint32_t maxParticleNumber)
{
	m_randGenerator = Xoshiro128(static_cast<uint32_t>(std::time(nullptr)));
	m_particleMesh.MakeParticleMesh(maxParticleNumber);

	m_particlePositions.resize(maxParticleNumber);
	m_particleVelocities.resize(maxParticleNumber);
	m_particleSizes.resize(maxParticleNumber);
	m_particleLifes.resize(maxParticleNumber);
	m_colors.resize(maxParticleNumber);
	m_particleTextureIndex.resize(maxParticleNumber);
	m_particleFrictions.resize(maxParticleNumber);
	m_particleIsGravityAffected.resize(maxParticleNumber);

	m_particles.m_positions = &m_particlePositions;
	m_particles.m_velocities = &m_particleVelocities;
	m_particles.m_sizes = &m_particleSizes;
	m_particles.m_lifes = &m_particleLifes;
	m_particles.m_colors = &m_colors;
	m_particles.m_textureIndex = &m_particleTextureIndex;
	m_particles.m_frictions = &m_particleFrictions;
	m_particles.m_isGravityAffected = &m_particleIsGravityAffected;
}

void ParticleManager::UpdateParticles(float dt)
{
	ComputeUpdate(dt);

	if (m_particlesAlive != 0)
		ComputeDeadParticles();

	ComputeParticleSpawn(dt);
}

Mesh& ParticleManager::GetParticleMesh()
{
	return m_particleMesh;
}

Particles* ParticleManager::GetParticlesData()
{
	return &m_particles;
}

uint32_t ParticleManager::GetAliveParticleNumber()
{
	return m_particlesAlive;
}

uint32_t ParticleManager::AddParticleEmitor(ParticleEmitor particleEmitor)
{
	m_particleEmitors.push_back(particleEmitor);
	return static_cast<uint32_t>(m_particleEmitors.size() - 1);
}

void ParticleManager::ToggleParticleEmitor(uint32_t particleEmitorIndex)
{
	if (m_particleEmitors.empty() || particleEmitorIndex > m_particleEmitors.size() - 1)
		return; // TODO err
	m_particleEmitors[particleEmitorIndex].m_isEmitorEnabled = !m_particleEmitors[particleEmitorIndex].m_isEmitorEnabled;
}

void ParticleManager::SetParticleEmitorPosition(uint32_t particleEmitorIndex, glm::vec3 position)
{
	if (m_particleEmitors.empty() || particleEmitorIndex > m_particleEmitors.size() - 1)
		return; // TODO err
	m_particleEmitors[particleEmitorIndex].m_emitorPosition = position;
}

void ParticleManager::ComputeDeadParticles()
{
	for (uint32_t i = m_particlesAlive; i-- > 0;)
	{
		glm::vec2 life = m_particleLifes[i];
		if (life.x > life.y)
		{
			uint32_t lastAliveIndex = m_particlesAlive - 1;
			m_particlesAlive--;

			if (lastAliveIndex == i)
				continue;

			std::swap(m_particlePositions[i], m_particlePositions[lastAliveIndex]);
			std::swap(m_particleVelocities[i], m_particleVelocities[lastAliveIndex]);
			std::swap(m_particleSizes[i], m_particleSizes[lastAliveIndex]);
			std::swap(m_particleLifes[i], m_particleLifes[lastAliveIndex]);
			std::swap(m_colors[i], m_colors[lastAliveIndex]);
			std::swap(m_particleTextureIndex[i], m_particleTextureIndex[lastAliveIndex]);
			std::swap(m_particleFrictions[i], m_particleFrictions[lastAliveIndex]);
			std::vector<bool>::swap(m_particleIsGravityAffected[i], m_particleIsGravityAffected[lastAliveIndex]);
		}
	}
}

void ParticleManager::ComputeParticleSpawn(float dt)
{
	for (size_t i = 0; i < m_particleEmitors.size(); i++)
	{
		ParticleEmitor& pe = m_particleEmitors[i];
		if (!pe.m_isEmitorEnabled)
			continue;

		pe.m_spawnAccumulator += pe.m_emissionRate * dt;

		while (pe.m_spawnAccumulator >= 1.f)
		{
			if (m_particlesAlive == m_particlePositions.size())
				break;

			glm::vec3 range = pe.m_offsetPositionMax - pe.m_offsetPositionMin;
			glm::vec3 randomOffset = range * m_randGenerator.RandomVec3();
			m_particlePositions[m_particlesAlive] = pe.m_emitorPosition + pe.m_offsetPositionMin + randomOffset;

			float speed = pe.m_speedMin + (pe.m_speedMax - pe.m_speedMin) * m_randGenerator.RandomFloat();
			glm::vec3 dir = pe.m_directionsMin + (pe.m_directionsMax - pe.m_directionsMin) * m_randGenerator.RandomVec3();
			m_particleVelocities[m_particlesAlive] = dir * speed;

			m_particleSizes[m_particlesAlive] = pe.m_sizeMin + (pe.m_sizeMax - pe.m_sizeMin) * m_randGenerator.RandomVec2();

			m_particleLifes[m_particlesAlive].x = 0.f;
			m_particleLifes[m_particlesAlive].y = pe.m_lifeMin + (pe.m_lifeMax - pe.m_lifeMin) * m_randGenerator.RandomFloat();

			m_particleTextureIndex[m_particlesAlive] = pe.m_textureIndex;
			m_colors[m_particlesAlive].first = pe.m_colorBegin;
			m_colors[m_particlesAlive].second = pe.m_colorEnd;

			m_particleFrictions[m_particlesAlive] = pe.m_friction;

			m_particleIsGravityAffected[m_particlesAlive] = pe.m_isGravityAffected;

			m_particlesAlive++;
			pe.m_spawnAccumulator--;
		}

		if (m_particlesAlive == m_particlePositions.size())
			break; //TODO tell too much spawn for the current max particles
	}
}

void ParticleManager::ComputeUpdate(float dt)
{
	for (uint32_t i = 0; i < m_particlesAlive; i++)
	{
		m_particlePositions[i] += m_particleVelocities[i] * dt;
		m_particleLifes[i].x += dt;
		m_particleVelocities[i] -= m_particleVelocities[i] * m_particleFrictions[i] * dt;

		if (m_particleIsGravityAffected[i])
			m_particleVelocities[i].y -= 9.81f * dt;
	}
}
