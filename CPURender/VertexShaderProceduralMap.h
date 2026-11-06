#pragma once
#include "IVertexShader.h"
#include "PerlinNoise.h"

struct GerstnerWave {
    glm::vec2 direction;
    float amplitude;
    float wavelength;
    float steepness;
    float speed;
    float phase;
};

class VertexShaderProceduralMap: public IVertexShader
{
public:
    VertexShaderProceduralMap();
	VertexOutput ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer & constantBuffer, uint32_t vertexIndex) override;

private:
    float GerstnerWaveHeight(GerstnerWave& wave, float x, float z, float t);
    float GerstnerWavesHeight(GerstnerWave* waves, int count, float x, float z, float t);
    glm::vec3 GerstnerWavesNormal(GerstnerWave* waves, int count, float x, float z, float t);

private:
	float m_waterLevel = (MINHEIGHT + MIDHEIGHT) * 0.5f;
	float m_waterUVSpeedOffset = 0.001f;
    GerstnerWave m_waves[6];
};