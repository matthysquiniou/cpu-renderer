#include "PixelShaderSimple.h"
#include "RenderAPI.h"
#include "Utils.h"

uint32_t PixelShaderSimple::ComputePixel(
    glm::vec3 position, glm::vec3 normal,
    glm::vec2 uv, glm::vec2 pixelCoords,
    uint32_t triangleIndex, ConstantBuffer& constantBuffer,
    Texture* texture, CubeMap* cubeMap 
    ,Texture * textureSpecular
)
{
    glm::vec3 albedo;
    glm::vec3 specular;

    float shininess = 8.0f; 
    float specularStrength = 0.0f;

    bool HaveSpecular = false;

    if (texture == nullptr)
        albedo = glm::vec3(1, 1, 1);
    else
    {
        albedo = Utils::Uint32ToVec3(RenderAPI::SampleTexture(*texture, uv));
        shininess = texture->m_shininess;
        specularStrength = texture->m_specularStrength;
    }
    if (textureSpecular != nullptr)
    {
        specular = Utils::Uint32ToVec3(RenderAPI::SampleTexture(*textureSpecular, uv));
        HaveSpecular = true;
    }
  
    glm::vec3 worldNormal = glm::normalize(glm::mat3(constantBuffer.m_InvTransposeModel) * normal);
    glm::vec3 viewDir = glm::normalize(constantBuffer.m_cameraPos - position);

    glm::vec3 accumulatedDiffuse = glm::vec3(0);
    glm::vec3 accumulatedSpecular = glm::vec3(0);

    glm::vec3 ambientColor = constantBuffer.m_lightsData->m_ambiantLight->m_color * constantBuffer.m_lightsData->m_ambiantLight->m_intensity;
    glm::vec3 lightDir;
    glm::vec3 lightColor;

    for (DirectionalLight& dl : *constantBuffer.m_lightsData->m_directionalLights)
    {
        ComputeDirectionalLight(lightDir, lightColor, dl);
        AccumulateLight(lightDir, lightColor, accumulatedDiffuse, accumulatedSpecular, worldNormal, viewDir, shininess, specularStrength,specular,HaveSpecular);
      
    }

    for (PointLight& pl : *constantBuffer.m_lightsData->m_pointLights)
    {
        ComputePointLight(lightDir, lightColor, pl, position);
        AccumulateLight(lightDir, lightColor, accumulatedDiffuse, accumulatedSpecular, worldNormal, viewDir, shininess, specularStrength, specular, HaveSpecular);
    }

    for (SpotLight& sl : *constantBuffer.m_lightsData->m_spotLights)
    {
        ComputeSpotLight(lightDir, lightColor, sl, position);
        AccumulateLight(lightDir, lightColor, accumulatedDiffuse, accumulatedSpecular, worldNormal, viewDir, shininess, specularStrength, specular, HaveSpecular);
    }

    glm::vec3 finalColor = (ambientColor + accumulatedDiffuse) * albedo + accumulatedSpecular;
    finalColor = finalColor / (finalColor + 1.f);

    return Utils::Vec3ToUint32(finalColor);
}

void PixelShaderSimple::ComputeDirectionalLight(glm::vec3& lightDir, glm::vec3& lightColor, DirectionalLight& directionalLight)
{
    lightColor = directionalLight.m_color * directionalLight.m_intensity;
    lightDir = normalize(-directionalLight.m_dir);
}

void PixelShaderSimple::ComputePointLight(glm::vec3& lightDir, glm::vec3& lightColor, PointLight& pointLight, glm::vec3& vertexPostion)
{
    glm::vec3 vPos = glm::vec3(vertexPostion);
    lightColor = pointLight.m_color * pointLight.m_intensity;

    glm::vec3 toLight = pointLight.m_position - vPos;
    float distance = std::sqrt(glm::dot(toLight, toLight));
    lightDir = toLight / distance;

    float attenuation = glm::clamp(1.0f - distance / pointLight.m_range, 0.f, 1.f);
    lightColor *= attenuation;
}

void PixelShaderSimple::ComputeSpotLight(glm::vec3& lightDir, glm::vec3& lightColor, SpotLight& spotLight, glm::vec3& vertexPostion) // spotlight may not be working 
{
    glm::vec3 vPos = glm::vec3(vertexPostion);
    lightColor = spotLight.m_color * spotLight.m_intensity;

    glm::vec3 toLight = spotLight.m_position - vPos;
    float distance = std::sqrt(glm::dot(toLight, toLight));
    lightDir = toLight / distance;

    float theta = dot(lightDir, normalize(-spotLight.m_dir));
    float diff = spotLight.m_beginAngleRad - spotLight.m_endAngleRad;
    float intensity = glm::clamp((theta - spotLight.m_endAngleRad) / diff, 0.f, 1.f);

    float attenuation = glm::clamp(1.0f - distance / spotLight.m_range, 0.f, 1.f); 
    lightColor *= attenuation * intensity;
}

void PixelShaderSimple::AccumulateLight(
    glm::vec3& lightDir, glm::vec3& lightColor,
    glm::vec3& accumulatedDiffuse, glm::vec3& accumulatedSpecular,
    glm::vec3& worldNormal, glm::vec3& viewDir,
    float shininess, float specularStrength,
     glm::vec3& Specular ,bool haveSpecular
)
{
    float diffIntensity = std::max(glm::dot(worldNormal, lightDir), 0.0f);
    accumulatedDiffuse += lightColor * diffIntensity;

    if (diffIntensity > 0.0f)
    {
        glm::vec3 halfDir = glm::normalize(lightDir + viewDir);
        float specIntensity = std::pow(
            std::max(glm::dot(worldNormal, halfDir), 0.0f),
            shininess
        );
        if (haveSpecular)
            accumulatedSpecular += lightColor * specIntensity * Specular; 
        else
        accumulatedSpecular += lightColor * specIntensity * specularStrength;
    }
}

glm::vec3 PixelShaderSimple::LerpSpecular()
{
    return glm::vec3();
}
