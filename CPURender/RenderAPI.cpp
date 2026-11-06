#include "RenderAPI.h"
#include <iostream>
#include "Logger.h"

std::vector<Texture> RenderAPI::s_textures;

RenderAPI::RenderAPI()
{
    const size_t size = static_cast<size_t>(WIDTH) * static_cast<size_t>(HEIGHT);
    m_depthBuffer.assign(size, 0.0f);
}

uint32_t RenderAPI::CreateTexture(std::string path)
{
    Texture texture = Texture();
    texture.InitImageTexture(path);
    uint32_t handleId = m_nextHandleId++;
    m_mapTextureHandleToIndex[handleId] = static_cast<uint32_t>(s_textures.size());
    s_textures.push_back(std::move(texture));
    return handleId;
}

uint32_t RenderAPI::CreateCubeMap(std::string paths[6])
{
    CubeMap cubeMap = CubeMap();
    cubeMap.LoadAllTextures(paths);
    uint32_t handleId = m_nextHandleId++;
    m_mapCubeMapHandleToIndex[handleId] = static_cast<uint32_t>(m_cubeMap.size());
    m_cubeMap.push_back(std::move(cubeMap));
    return handleId;
}

uint32_t RenderAPI::CreateSpecularTexture(std::string path)
{
    Texture texture = Texture();
    texture.InitImageTexture(path);
    uint32_t handleId = m_nextHandleId++;
    m_mapTextureHandleToIndex[handleId] = static_cast<uint32_t>(s_textures.size());
    s_textures.push_back(std::move(texture));
    return handleId;
}

uint32_t RenderAPI::CreateVertexBuffer()
{
    uint32_t handleId = m_nextHandleId++;
    Vertices vertices;
    m_mapVBHandleToIndex[handleId] = static_cast<uint32_t>(m_vertexBuffer.size());
    m_vertexBuffer.push_back(vertices);
    return handleId;
}

uint32_t RenderAPI::CreateConstantBuffer()
{
    uint32_t handleId = m_nextHandleId++;
    ConstantBuffer constantBuffer;
    m_mapCBHandleToIndex[handleId] = static_cast<uint32_t>(m_constantBuffer.size());
    m_constantBuffer.push_back(constantBuffer);
    return handleId;
}

uint32_t RenderAPI::GetTextureIndex(uint32_t textureHandle)
{
    auto itTexture = m_mapTextureHandleToIndex.find(textureHandle);
    if (itTexture == m_mapTextureHandleToIndex.end())
        return 0; // TODO err

    return itTexture->second;
}

void RenderAPI::UploadTextureData(uint32_t handle, std::vector<uint32_t>& textureData)
{
    auto it = m_mapTextureHandleToIndex.find(handle);
    if (it == m_mapTextureHandleToIndex.end())
    {
        return; //TODO raise error
    }

    Texture& texture = s_textures[it->second];
    texture.SetTexture(textureData);
}


void RenderAPI::UploadTextureSpecular(uint32_t handle, float shininess, float specularStrength)
{
    auto it = m_mapTextureHandleToIndex.find(handle);
    if (it == m_mapTextureHandleToIndex.end())
    {
        return; //TODO raise error
    }

    Texture& texture = s_textures[it->second];
    texture.m_shininess = shininess;
    texture.m_specularStrength = specularStrength;
}

void RenderAPI::UploadVertexBuffer(uint32_t handle, std::vector<glm::vec3>& coordinates, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs)
{
    auto it = m_mapVBHandleToIndex.find(handle);
    if (it == m_mapVBHandleToIndex.end())
    {
        return; //TODO raise error
    }
    // TODO check if vertex mod 3 = 0 but maybe latter particles will have a problem with that so not atm
    Vertices& vertexBuffer = m_vertexBuffer[it->second];
    vertexBuffer.SetVertices(coordinates, normals, uvs);
}

void RenderAPI::UploadConstantBuffer(uint32_t handle, ConstantBuffer& constantBuffer)
{
    auto it = m_mapCBHandleToIndex.find(handle);
    if (it == m_mapCBHandleToIndex.end())
    {
        return; //TODO raise error
    }

    ConstantBuffer& cb = m_constantBuffer[it->second];
    cb = constantBuffer;
}

void RenderAPI::AddVertexBufferToDraw(uint32_t vertexBufferHandle, uint32_t constantBufferHandle, uint32_t vertexShaderHandle, uint32_t pixelShaderHandle, uint32_t textureAlbedoHandle, uint32_t cubeMapHandle, uint32_t textureSpecularHandle, bool cullClockWise)
{

    auto itVB = m_mapVBHandleToIndex.find(vertexBufferHandle);
    auto itCB = m_mapCBHandleToIndex.find(constantBufferHandle);
    auto itVS = m_mapVSHandleToIndex.find(vertexShaderHandle);
    auto itPS = m_mapPSHandleToIndex.find(pixelShaderHandle);
    if (
        itVB == m_mapVBHandleToIndex.end() ||
        itCB == m_mapCBHandleToIndex.end() ||
        itVS == m_mapVSHandleToIndex.end() ||
        itPS == m_mapPSHandleToIndex.end()
        )
        return; //TODO raise error

    Texture* textureAlbedo = nullptr;
    CubeMap* cubeMap = nullptr;
    Texture* specularTexture = nullptr;

    auto itTexture = m_mapTextureHandleToIndex.find(textureAlbedoHandle);
    auto itCubeMap = m_mapCubeMapHandleToIndex.find(cubeMapHandle);
    auto itspecularMap = m_mapTextureHandleToIndex.find(textureSpecularHandle);

    if (itTexture != m_mapTextureHandleToIndex.end())
        textureAlbedo = &s_textures[itTexture->second];

    if (itCubeMap != m_mapCubeMapHandleToIndex.end())
        cubeMap = &m_cubeMap[itCubeMap->second];

    if (itspecularMap != m_mapTextureHandleToIndex.end())
        specularTexture = &s_textures[itspecularMap->second];

    m_drawCommands.push_back
    ({ m_vertexBuffer[itVB->second], m_constantBuffer[itCB->second], 
        m_vertexShaders[itVS->second].get(), m_pixelShaders[itPS->second].get(),
        textureAlbedo, cubeMap, specularTexture, cullClockWise});
}

void RenderAPI::FlushDraw(FinalFrameBuffer& frameBuffer)
{
    VertexShaderPass();
    ClippingPass();
    BackfaceCullingPass();
    RasterizingPass(frameBuffer);
    m_drawCommands.clear();
    m_drawCommandIndexByTriangle.clear();
}

uint32_t RenderAPI::SampleTexture(Texture& texture, glm::vec2 uv)
{
    return texture.Sample(uv.x, uv.y);
}

uint32_t RenderAPI::SampleTexture(uint32_t textureIndex, glm::vec2 uv)
{
    return s_textures[textureIndex].Sample(uv.x, uv.y);
}

uint32_t RenderAPI::SampleSpecularMap(uint32_t specularIndex, glm::vec2 uv)
{
    return s_textures[specularIndex].Sample(uv.x, uv.y);
}
glm::vec2 RenderAPI::GetTextureSpecularInfo(uint32_t textureIndex)
{
    return glm::vec2(s_textures[textureIndex].m_shininess, s_textures[textureIndex].m_specularStrength);
}

uint32_t RenderAPI::SampleCubeMap(CubeMap& cubeMap, glm::vec3 direction)
{
    return cubeMap.Sample(direction);
}

void RenderAPI::VertexShaderPass()
{
    uint32_t totalVertexCount = 0;
    for (size_t drawCommandIndex = 0; drawCommandIndex < m_drawCommands.size(); drawCommandIndex++)
    {
        DrawCommand& drawCommand = m_drawCommands[drawCommandIndex];
        ConstantBuffer& constantBuffer = drawCommand.m_constantBuffer;
        if (constantBuffer.m_particles != nullptr)
        {
            totalVertexCount += constantBuffer.m_particleAlive * 6;
        }
        else
        {
            Vertices& vertices = drawCommand.m_vertexBuffer;
            totalVertexCount += static_cast<uint32_t>(vertices.m_positions.size());
        }
    }
    uint32_t totalTriangleCount = totalVertexCount / 3;

    m_vertexShaderOut.Resize(totalVertexCount);
    m_drawCommandIndexByTriangle.resize(totalTriangleCount);

    uint32_t vertexOffset = 0;
    uint32_t triangleOffset = 0;

    for (size_t drawCommandIndex = 0; drawCommandIndex < m_drawCommands.size(); drawCommandIndex++)
    {
        DrawCommand& drawCommand = m_drawCommands[drawCommandIndex];

        IVertexShader& vertexShader = *drawCommand.m_vertexShader;
        Vertices& vertices = drawCommand.m_vertexBuffer;
        ConstantBuffer& constantBuffer = drawCommand.m_constantBuffer;

        uint32_t localVertexCount;
        if (constantBuffer.m_particles != nullptr)
            localVertexCount = constantBuffer.m_particleAlive * 6;
        else 
            localVertexCount = static_cast<uint32_t>(vertices.m_positions.size());

        uint32_t localTriangleCount = localVertexCount / 3;

        for (uint32_t localTriangleIndex = 0; localTriangleIndex < localTriangleCount; ++localTriangleIndex)
        {
            uint32_t globalTriangleIndex = localTriangleIndex + triangleOffset;
            m_drawCommandIndexByTriangle[globalTriangleIndex] = static_cast<uint32_t>(drawCommandIndex);
            for (int triangleVertexIndex = 0; triangleVertexIndex < 3; ++triangleVertexIndex)
            {
                uint32_t localVertexIndex = localTriangleIndex * 3 + triangleVertexIndex;
                uint32_t globalVertexIndex = vertexOffset + localVertexIndex;
                VertexOutput vertexOutput = vertexShader.ComputeVertex(vertices.m_positions[localVertexIndex], vertices.m_normals[localVertexIndex], vertices.m_uvs[localVertexIndex], constantBuffer, localVertexIndex);
                m_vertexShaderOut.AddVertexOutputAt(vertexOutput, globalVertexIndex);
            }
        }

        vertexOffset += localVertexCount;
        triangleOffset += localTriangleCount;
    }
}

void RenderAPI::BackfaceCullingPass()
{
    uint32_t triangleCount = static_cast<uint32_t> (m_clippedTriangles.size());

    m_backfaceTriangles.reserve(triangleCount);
    for (uint32_t triIndex = 0; triIndex < triangleCount; triIndex++)
    {
        DrawCommand& drawCommand = m_drawCommands[m_clippedTriangles[triIndex].m_drawCommandIndex];
        glm::vec2 a = m_clippedTriangles[triIndex].m_v[0].m_screenPos;
        glm::vec2 b = m_clippedTriangles[triIndex].m_v[1].m_screenPos;
        glm::vec2 c = m_clippedTriangles[triIndex].m_v[2].m_screenPos;

        float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

        
        if (cross >= 0 && !drawCommand.m_cullClockWise || cross <= 0 && drawCommand.m_cullClockWise)
            continue;
        RasterTriangle& triangle = m_clippedTriangles[triIndex];
        m_backfaceTriangles.push_back(triangle);
    }
    m_rasterizer.Update(m_backfaceTriangles);
}

void RenderAPI::ClippingPass()
{
    uint32_t triangleCount = static_cast<uint32_t>(m_drawCommandIndexByTriangle.size());
    m_clippedTriangles.clear();
    m_clippedTriangles.reserve(triangleCount + triangleCount / 4); // can technically create triangle up to * 4 but most of the time no

    VerticesOutput input;
    VerticesOutput output;
    input.Reserve(18);
    output.Reserve(18);

    uint32_t drawCommandIndex;
    uint32_t lastDrawCommandIndex = -1;
    uint32_t drawCommandTriangleIndex = 0;

    for (uint32_t triIdx = 0; triIdx < triangleCount; ++triIdx)
    {
        drawCommandTriangleIndex++;

        drawCommandIndex = m_drawCommandIndexByTriangle[triIdx];
        if (drawCommandIndex != lastDrawCommandIndex)
        {
            lastDrawCommandIndex = drawCommandIndex;
            drawCommandTriangleIndex = 0;
        }

        static glm::vec4 equations[6] =
        {
            { 0.f,  0.f,  1.f, 1.f}, // near  
            { 0.f,  0.f, -1.f, 1.f}, // far   
            { 1.f,  0.f,  0.f, 1.f}, // left  
            {-1.f,  0.f,  0.f, 1.f}, // right 
            { 0.f,  1.f,  0.f, 1.f}, // bottom
            { 0.f, -1.f,  0.f, 1.f}, // top   
        };

        input.Clear();
        input.AddVertexOutputAt(m_vertexShaderOut, triIdx * 3, 0);
        input.AddVertexOutputAt(m_vertexShaderOut, triIdx * 3 + 1, 1);
        input.AddVertexOutputAt(m_vertexShaderOut, triIdx * 3 + 2, 2);


        for (glm::vec4 equation : equations)
        {
            output.Clear();
            for (uint32_t i = 0; i < input.m_clipPositions.size(); i += 3)
                ClipTriangle(i, equation, input, output);

            std::swap(input, output);
        }

        for (uint32_t indexClipRes = 0; indexClipRes < input.m_clipPositions.size(); indexClipRes+=3)
        {
            bool valid = true;
            RasterTriangle rasterTriangle;

            for (uint32_t localVertexIndex = 0; localVertexIndex < 3; localVertexIndex++)
            {
                uint32_t vIdx = indexClipRes + localVertexIndex;
                glm::vec4& clip = input.m_clipPositions[vIdx];

                if (clip.w <= 0.0f)
                {
                    valid = false;
                    break;
                }

                float invW = 1.0f / clip.w;
                glm::vec3 ndc = glm::vec3(clip) * invW;

                RasterVertexData& rasterVertexData = rasterTriangle.m_v[localVertexIndex];
                rasterVertexData.m_screenPos = glm::vec2((ndc.x * 0.5f + 0.5f) * static_cast<float>(WIDTH), (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(HEIGHT));
                rasterVertexData.m_invW = invW;
                rasterVertexData.m_worldPos = input.m_worldPositions[vIdx];
                rasterVertexData.m_normal = input.m_normals[vIdx];
                rasterVertexData.m_uv = input.m_uvs[vIdx];
                rasterVertexData.m_triangleIndex = drawCommandTriangleIndex;
            }

            if (!valid)
                continue;

            rasterTriangle.m_drawCommandIndex = m_drawCommandIndexByTriangle[triIdx];
            rasterTriangle.m_invArea2 = 0.0f;

            m_clippedTriangles.push_back(rasterTriangle);
        }
    }

}

void RenderAPI::RasterizingPass(FinalFrameBuffer& frameBuffer)
{
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), 0.0f);
    m_rasterizer.Render(m_backfaceTriangles, m_pixelShaders, s_textures, m_constantBuffer,m_drawCommands,frameBuffer, m_depthBuffer);
    m_backfaceTriangles.clear();
}

void RenderAPI::ClipTriangle(uint32_t index, glm::vec4& equation, VerticesOutput& input, VerticesOutput& output) // https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer-part-5.html
{
    uint32_t index2 = index + 1;
    uint32_t index3 = index + 2;

    float values[3] =
    {
        glm::dot(input.m_clipPositions[index], equation),
        glm::dot(input.m_clipPositions[index2], equation),
        glm::dot(input.m_clipPositions[index3], equation),
    }; 

    std::uint8_t mask = (values[0] < 0.f ? 1 : 0) | (values[1] < 0.f ? 2 : 0) | (values[2] < 0.f ? 4 : 0);

    switch (mask)
    {
    case 0b000:
        output.AddVertexOutput(input, index);
        output.AddVertexOutput(input, index2);
        output.AddVertexOutput(input, index3);
        break;
    case 0b001:
    {
        VertexOutput v01 = ClipIntersectEdge(input, index, index2, values[0], values[1]);
        VertexOutput v02 = ClipIntersectEdge(input, index, index3, values[0], values[2]);
        output.AddVertexOutput(v01); 
        output.AddVertexOutput(input, index2);
        output.AddVertexOutput(input, index3); 
        output.AddVertexOutput(v01);
        output.AddVertexOutput(input, index3);
        output.AddVertexOutput(v02); 
    }
    break;
    case 0b010:
    {
        VertexOutput v10 = ClipIntersectEdge(input, index2, index, values[1], values[0]);
        VertexOutput v12 = ClipIntersectEdge(input, index2, index3, values[1], values[2]);
        output.AddVertexOutput(input, index);   
        output.AddVertexOutput(v10);            
        output.AddVertexOutput(v12);            
        output.AddVertexOutput(input, index);   
        output.AddVertexOutput(v12);            
        output.AddVertexOutput(input, index3);  
    }
    break;
    case 0b011:
    {
        VertexOutput v1 = ClipIntersectEdge(input, index, index3, values[0], values[2]);
        VertexOutput v2 = ClipIntersectEdge(input, index2, index3, values[1], values[2]);
        output.AddVertexOutput(v1);
        output.AddVertexOutput(v2);
        output.AddVertexOutput(input, index3);
    }
    break;
    case 0b100:
    {
        VertexOutput v20 = ClipIntersectEdge(input, index3, index, values[2], values[0]);
        VertexOutput v21 = ClipIntersectEdge(input, index3, index2, values[2], values[1]);
        output.AddVertexOutput(v20);
        output.AddVertexOutput(input, index);
        output.AddVertexOutput(input, index2);
        output.AddVertexOutput(v20);
        output.AddVertexOutput(input, index2);
        output.AddVertexOutput(v21);
    }
    break;
    case 0b101:
    {
        VertexOutput v0 = ClipIntersectEdge(input, index, index2, values[0], values[1]);
        VertexOutput v2 = ClipIntersectEdge(input, index3, index2, values[2], values[1]);
        output.AddVertexOutput(v0);
        output.AddVertexOutput(input, index2);
        output.AddVertexOutput(v2);
    }
    break;
    case 0b110:
    {
        VertexOutput v1 = ClipIntersectEdge(input, index2, index, values[1], values[0]);
        VertexOutput v2 = ClipIntersectEdge(input, index3, index, values[2], values[0]);
        output.AddVertexOutput(input, index);
        output.AddVertexOutput(v1);
        output.AddVertexOutput(v2);
    }
    break;
    case 0b111:
        break;

    default:
        break;
    }
}


VertexOutput RenderAPI::ClipIntersectEdge(VerticesOutput& result, uint32_t indexV0, uint32_t indexV1, float value0, float value1)
{
    float t = value0 / (value0 - value1);

    VertexOutput v;
    v.m_clipPosition = (1.f - t) * result.m_clipPositions[indexV0] + t * result.m_clipPositions[indexV1];
    v.m_normal = (1.f - t) * result.m_normals[indexV0] + t * result.m_normals[indexV1];
    v.m_uv = (1.f - t) * result.m_uvs[indexV0] + t * result.m_uvs[indexV1];
    v.m_worldPosition = (1.f - t) * result.m_worldPositions[indexV0] + t * result.m_worldPositions[indexV1];

    return v;
}