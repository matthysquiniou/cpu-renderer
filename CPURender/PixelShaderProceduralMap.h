#pragma once
#include "IPixelShader.h"

class PixelShaderProceduralMap : public IPixelShader
{
    struct TextureInfo
    {
        glm::vec3 color;
        glm::vec2 shininessAndSpecStrength;
    };

    uint32_t ComputePixel(
        glm::vec3 position, glm::vec3 normal,
        glm::vec2 uv, glm::vec2 pixelCoords,
        uint32_t triangleIndex, ConstantBuffer& constantBuffer,
        Texture* texture, CubeMap* cubeMap, Texture* textureSpecular
    ) override;

    TextureInfo GetTextureInfo(glm::vec3 position, glm::vec3 normal, glm::vec2 uv,ConstantBuffer& constantBuffer, Texture* texture);
    glm::vec3 Lerp(float position, float value1, float value2, int textureindex, glm::vec2 uv, ConstantBuffer& constantBuffer);
    void ComputeDirectionalLight(glm::vec3& lightDir, glm::vec3& lightColor, DirectionalLight& directionalLight);
    void ComputePointLight(glm::vec3& lightDir, glm::vec3& lightColor, PointLight& pointLight, glm::vec3& vertexPostion);
    void ComputeSpotLight(glm::vec3& lightDir, glm::vec3& lightColor, SpotLight& spotLight, glm::vec3& vertexPostion);
    void AccumulateLight(
        glm::vec3& lightDir, glm::vec3& lightColor,
        glm::vec3& accumulatedDiffuse, glm::vec3& accumulatedSpecular,
        glm::vec3& worldNormal, glm::vec3& viewDir,
        float shininess, float specularStrength,
        glm::vec3& specularaccumulate, bool haveSpecular
    );


};

