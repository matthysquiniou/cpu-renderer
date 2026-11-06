#pragma once
#include "IPixelShader.h"

class PixelShaderSkyBox : public IPixelShader
{
public:
    uint32_t ComputePixel(
        glm::vec3 position, glm::vec3 normal,
        glm::vec2 uv, glm::vec2 pixelCoords,
        uint32_t triangleIndex, ConstantBuffer& constantBuffer,
        Texture* texture, CubeMap* cubeMap,Texture * textureSpecular
    ) override;
};