#include "PixelShaderSkybox.h"
#include "RenderAPI.h"

uint32_t PixelShaderSkyBox::ComputePixel(
    glm::vec3 position, glm::vec3 normal,
    glm::vec2 uv, glm::vec2 pixelCoords, 
    uint32_t triangleIndex, ConstantBuffer& constantBuffer, 
    Texture* texture, CubeMap* cubeMap,
    Texture* textureSpecular
)
{
    glm::vec3 direction = glm::normalize(position - constantBuffer.m_cameraPos);
    return RenderAPI::SampleCubeMap(*cubeMap, direction);
}
