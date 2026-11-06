#include "VertexShaderSimple.h"

VertexOutput VertexShaderSimple::ComputeVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, ConstantBuffer& constantBuffer, uint32_t vertexIndex)
{
	VertexOutput out;
    glm::vec4 worldPos = glm::vec4(position, 1.0f);
    out.m_clipPosition = constantBuffer.m_viewProj * constantBuffer.m_model * worldPos;
    out.m_worldPosition = glm::vec3(worldPos);
    out.m_normal = normal;
    out.m_uv = uv;
    return out;
}
