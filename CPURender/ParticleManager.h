#pragma once
#include "ParticleEmitor.h"
#include "Particles.h"
#include "Mesh.h"
#include "Xoshiro128.h"

class ParticleManager
{
public:
	ParticleManager(uint32_t maxParticleNumber);

	void UpdateParticles(float dt);

	Mesh& GetParticleMesh();
	Particles* GetParticlesData();
	uint32_t GetAliveParticleNumber();
	
	uint32_t AddParticleEmitor(ParticleEmitor particleEmitor);

	void ToggleParticleEmitor(uint32_t particleEmitorIndex);
	void SetParticleEmitorPosition(uint32_t particleEmitorIndex, glm::vec3 position);

private:
	void ComputeDeadParticles();
	void ComputeParticleSpawn(float dt);
	void ComputeUpdate(float dt);
private:
	Mesh m_particleMesh;
	Particles m_particles;
	uint32_t m_particlesAlive = 0;

	std::vector<glm::vec3> m_particlePositions;
	std::vector<glm::vec3> m_particleVelocities;
	std::vector<glm::vec2> m_particleSizes;
	std::vector<glm::vec2> m_particleLifes;
	std::vector<std::pair<glm::vec3, glm::vec3>> m_colors;
	std::vector<uint32_t> m_particleTextureIndex;
	std::vector<float> m_particleFrictions;
	std::vector<bool> m_particleIsGravityAffected;

	std::vector<ParticleEmitor> m_particleEmitors;

	Xoshiro128 m_randGenerator;
};