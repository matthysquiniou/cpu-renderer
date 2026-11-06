#include "PixelShaderParticle.h"
#include "RenderAPI.h"
#include "Utils.h"

uint32_t PixelShaderParticle::ComputePixel(
    glm::vec3 position, glm::vec3 normal,
    glm::vec2 uv, glm::vec2 pixelCoords,
    uint32_t triangleIndex, ConstantBuffer& constantBuffer,
    Texture* texture, CubeMap* cubeMap,
    Texture* textureSpecular
)
{
    uint32_t particleIndex = triangleIndex >> 1;
    Particles& particles = *constantBuffer.m_particles;
    uint32_t textureIndex = (*particles.m_textureIndex)[particleIndex];

    glm::vec3 color;
    if (textureIndex == static_cast<uint32_t>(-1))
    {
        color = ColorLerp(constantBuffer, particleIndex);
        color = glm::vec3(color.z, color.y, color.x);
    }
    else
        color = Utils::Uint32ToVec3(RenderAPI::SampleTexture(textureIndex, uv));

    return Utils::Vec3ToUint32(glm::clamp(color, 0.f, 1.f));
}

glm::vec3 PixelShaderParticle::ColorLerp(ConstantBuffer& constantBuffer, uint32_t particleIndex)
{
    std::pair<glm::vec3, glm::vec3>& colors = (*constantBuffer.m_particles->m_colors)[particleIndex];
    glm::vec2 life = (*constantBuffer.m_particles->m_lifes)[particleIndex];
    float t = glm::clamp(life.x / life.y, 0.f, 1.f);
    return glm::mix(colors.first, colors.second, t);
}

