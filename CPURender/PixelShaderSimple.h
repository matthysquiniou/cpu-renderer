#pragma once
#include "IPixelShader.h"

class PixelShaderSimple : public IPixelShader
{
public:
    uint32_t ComputePixel(
        glm::vec3 position, glm::vec3 normal,
        glm::vec2 uv, glm::vec2 pixelCoords,
        uint32_t triangleIndex, ConstantBuffer& constantBuffer,
        Texture* texture, CubeMap* cubeMap, Texture* textureSpecular
    ) override;

private:
    void ComputeDirectionalLight(glm::vec3& lightDir, glm::vec3& lightColor, DirectionalLight& directionalLight);
    void ComputePointLight(glm::vec3& lightDir, glm::vec3& lightColor, PointLight& pointLight, glm::vec3& vertexPostion);
    void ComputeSpotLight(glm::vec3& lightDir, glm::vec3& lightColor, SpotLight&, glm::vec3& vertexPostion);

    void AccumulateLight(
        glm::vec3& lightDir, glm::vec3& lightColor,
        glm::vec3& accumulatedDiffuse, glm::vec3& accumulatedSpecular,
        glm::vec3& worldNormal, glm::vec3& viewDir,
        float shininess, float specularStrength,  
        glm::vec3& specularaccumulate , bool haveSpecular
    );
    glm::vec3 LerpSpecular();
};