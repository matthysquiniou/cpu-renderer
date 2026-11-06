#include "VertexShaderProceduralMap.h"
#include "PerlinNoise.h"

VertexShaderProceduralMap::VertexShaderProceduralMap()
{
    m_waves[0] = { glm::vec2(1.0000f,  0.0000f), 0.560f, 34.000f, 0.8f, 2.914f, 0.00f };
    m_waves[1] = { glm::vec2(-0.7374f,  0.6754f), 0.347f, 21.013f, 0.8f, 2.290f, 2.40f };
    m_waves[2] = { glm::vec2(0.0875f, -0.9962f), 0.214f, 12.987f, 0.8f, 1.801f, 4.80f };
    m_waves[3] = { glm::vec2(0.6085f,  0.7936f), 0.133f,  8.027f, 0.8f, 1.416f, 1.30f };
    m_waves[4] = { glm::vec2(-0.9847f, -0.1739f), 0.082f,  4.961f, 0.8f, 1.113f, 5.50f };
    m_waves[5] = { glm::vec2(0.8427f, -0.5384f), 0.051f,  3.066f, 0.8f, 0.876f, 3.70f };
}

VertexOutput VertexShaderProceduralMap::ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer& constantBuffer, uint32_t vertexIndex)
{
    VertexOutput out;
    glm::vec4 worldPos = glm::vec4(position, 1.0f);
    glm::vec3 worldNormal = normal;

    if (worldPos.y < m_waterLevel)
    {
        worldPos.y += GerstnerWavesHeight(m_waves, 6,worldPos.x,worldPos.z,constantBuffer.m_totalTime);
        worldNormal = GerstnerWavesNormal(m_waves, 6, worldPos.x, worldPos.z, constantBuffer.m_totalTime);

        out.m_uv = uv + uv * (constantBuffer.m_totalTime * m_waterUVSpeedOffset);
    }
    else
    {
        out.m_uv = uv;
    }
    out.m_clipPosition = constantBuffer.m_viewProj * constantBuffer.m_model * worldPos;
    out.m_worldPosition = glm::vec3(worldPos);
    out.m_normal = glm::normalize(glm::mat3(constantBuffer.m_InvTransposeModel) * worldNormal);;
    return out;
}

float VertexShaderProceduralMap::GerstnerWaveHeight(GerstnerWave& wave, float x, float z, float t) {
    float k = 2.0f * 3.14159265359f / wave.wavelength;

    float projection = wave.direction.x * x + wave.direction.y * z;
    float theta = k * projection - wave.speed * t + wave.phase;

    return wave.amplitude * std::sin(theta);
}

float VertexShaderProceduralMap::GerstnerWavesHeight(GerstnerWave* waves, int count, float x, float z, float t) {
    float totalY = 0.0f;
    for (int i = 0; i < count; ++i) {
        totalY += GerstnerWaveHeight(waves[i], x, z, t);
    }
    return totalY;
}

glm::vec3 VertexShaderProceduralMap::GerstnerWavesNormal(GerstnerWave* waves, int count, float x, float z, float t)
{
    float dhdx = 0.0f;
    float dhdz = 0.0f;

    for (int i = 0; i < count; ++i)
    {
        const float k = 2.0f * 3.14159265359f / waves[i].wavelength;
        const float projection = waves[i].direction.x * x + waves[i].direction.y * z;
        const float theta = k * projection - waves[i].speed * t + waves[i].phase;

        const float c = waves[i].amplitude * k * std::cos(theta);
        dhdx += c * waves[i].direction.x;
        dhdz += c * waves[i].direction.y;
    }

    return glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
}