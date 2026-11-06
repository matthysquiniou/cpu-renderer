#include "VertexShaderParticle.h"
#include "Utils.h"
#include "RenderAPI.h"

VertexOutput VertexShaderParticle::ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer& constantBuffer, uint32_t vertexIndex)
{
    VertexOutput out;
    uint32_t particleIndex = vertexIndex / 6;
    uint32_t particleCorner = vertexIndex % 6;
    glm::vec4 pos = glm::vec4(constantBuffer.m_particles->m_positions->at(particleIndex), 1.0f);
    glm::vec4 posInView = constantBuffer.m_view * pos;

    glm::vec2 size = constantBuffer.m_particles->m_sizes->at(particleIndex);

    switch (particleCorner)
    {
    case 0:
        posInView.x -= size.x * 0.5f;
        posInView.y += size.y * 0.5f;
        break;
    case 2:
    case 3:
        posInView.x += size.x * 0.5f;
        posInView.y += size.y * 0.5f;
        break;
    case 1:
    case 4:
        posInView.x -= size.x * 0.5f;
        posInView.y -= size.y * 0.5f;
        break;
    case 5:
        posInView.x += size.x * 0.5f;
        posInView.y -= size.y * 0.5f;
        break;
    }

    out.m_clipPosition = constantBuffer.m_proj * posInView;
    out.m_worldPosition = glm::vec3(pos);
    out.m_normal = normal;
    out.m_uv = uv;
    return out;
}
