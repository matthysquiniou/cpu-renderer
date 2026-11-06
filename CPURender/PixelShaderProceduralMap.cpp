#include "PixelShaderProceduralMap.h"
#include "RenderAPI.h"
#include "Utils.h"
#include  "PerlinNoise.h"
#include "Logger.h"

uint32_t PixelShaderProceduralMap::ComputePixel
    (glm::vec3 position, glm::vec3 normal,
    glm::vec2 uv, glm::vec2 pixelCoords, 
    uint32_t triangleIndex, ConstantBuffer& constantBuffer,
    Texture* texture, CubeMap* cubeMap,
    Texture* textureSpecular)
{
    glm::vec3 specular;

    bool HaveSpecular = false;


    if (textureSpecular != nullptr)
    {
        specular = Utils::Uint32ToVec3(RenderAPI::SampleTexture(*textureSpecular, uv));
        HaveSpecular = true;
    }
    PixelShaderProceduralMap::TextureInfo textureInfo;

    textureInfo = GetTextureInfo(position, normal,uv, constantBuffer, texture);
    float shininess = textureInfo.shininessAndSpecStrength.x;
    float specularStrength = textureInfo.shininessAndSpecStrength.y;

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

    glm::vec3 finalColor = (ambientColor + accumulatedDiffuse) * textureInfo.color + accumulatedSpecular;
    finalColor = finalColor / (finalColor + 1.f);

    return Utils::Vec3ToUint32(finalColor);
}

PixelShaderProceduralMap::TextureInfo PixelShaderProceduralMap::GetTextureInfo(glm::vec3 position, glm::vec3 normal, glm::vec2 uv,ConstantBuffer& constantBuffer, Texture* texture)
{

    float averageMinMid = (MINHEIGHT + MIDHEIGHT) * 0.5f;
    float averageMidHigh = (MIDHEIGHT + HIGHHEIGHT) * 0.5f;
    float averageHighMax = (MAXHEIGHT + HIGHHEIGHT) * 0.5f;

    if (position.y < averageMinMid)
        return  { Utils::Uint32ToVec3(RenderAPI::SampleTexture(constantBuffer.m_TexturesIndices[0], uv)), RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[0]) }; //water
    if (position.y < MIDHEIGHT)
        return { Lerp(position.y, averageMinMid, MIDHEIGHT, 1, uv, constantBuffer) , RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[1]) }; //water
    else if (position.y < averageMidHigh)
        return { Lerp(position.y,MIDHEIGHT , averageMidHigh, 2, uv, constantBuffer), RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[2]) }; //midGrass
    else if (normal.y > 0 && position.y < averageHighMax)
        return { Utils::Uint32ToVec3(RenderAPI::SampleTexture(constantBuffer.m_TexturesIndices[4], uv)), RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[3]) };
    else if (position.y < averageHighMax)
        return { Lerp(position.y, averageHighMax,HIGHHEIGHT, 4, uv, constantBuffer), RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[4]) }; //rock 
    else 
        return { Utils::Uint32ToVec3(RenderAPI::SampleTexture(constantBuffer.m_TexturesIndices[5], uv)), RenderAPI::GetTextureSpecularInfo(constantBuffer.m_TexturesIndices[5]) };
}

glm::vec3 PixelShaderProceduralMap::Lerp(float height, float value1, float value2, int textureIndex, glm::vec2 uv, ConstantBuffer& constantBuffer)
{
    float a = (height - value1) / (value2 - value1);
    a = glm::clamp(a, 0.0f, 1.0f);

    glm::vec3 colorlow = Utils::Uint32ToVec3(RenderAPI::SampleTexture(constantBuffer.m_TexturesIndices[textureIndex], uv));
    glm::vec3 colorHigh = Utils::Uint32ToVec3(RenderAPI::SampleTexture(constantBuffer.m_TexturesIndices[textureIndex + 1], uv));

    return  glm::mix(colorlow,colorHigh, a);
}

void PixelShaderProceduralMap::ComputeDirectionalLight(glm::vec3& lightDir, glm::vec3& lightColor, DirectionalLight& directionalLight)
{
    lightColor = directionalLight.m_color * directionalLight.m_intensity;
    lightDir = normalize(-directionalLight.m_dir);
}

void PixelShaderProceduralMap::ComputePointLight(glm::vec3& lightDir, glm::vec3& lightColor, PointLight& pointLight, glm::vec3& vertexPostion)
{
    glm::vec3 vPos = glm::vec3(vertexPostion);
    lightColor = pointLight.m_color * pointLight.m_intensity;

    glm::vec3 toLight = pointLight.m_position - vPos;
    float distance = std::sqrt(glm::dot(toLight, toLight));
    lightDir = toLight / distance;

    float attenuation = glm::clamp(1.0f - distance / pointLight.m_range, 0.f, 1.f);
    lightColor *= attenuation;
}

void PixelShaderProceduralMap::ComputeSpotLight(glm::vec3& lightDir, glm::vec3& lightColor, SpotLight& spotLight, glm::vec3& vertexPostion)
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

void PixelShaderProceduralMap::AccumulateLight(glm::vec3& lightDir, glm::vec3& lightColor, glm::vec3& accumulatedDiffuse, glm::vec3& accumulatedSpecular, glm::vec3& worldNormal, glm::vec3& viewDir, float shininess, float specularStrength, glm::vec3& specular, bool haveSpecular)
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
            accumulatedSpecular += lightColor * specIntensity * specular;
        else
            accumulatedSpecular += lightColor * specIntensity * specularStrength;
    }
}



