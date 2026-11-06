#include "Rasterizer.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "IPixelShader.h"
#include <algorithm>
#include <cmath>

Rasterizer::Rasterizer()
{
    m_tileTriangles.resize(CHUNKS_X * CHUNKS_Y);
}

void Rasterizer::Update(std::vector<RasterTriangle>& triangles)
{
    for (RasterTriangle& tri : triangles)
    {
        float area2 = CalculateArea(tri.m_v[0].m_screenPos, tri.m_v[1].m_screenPos, tri.m_v[2].m_screenPos);
        tri.m_invArea2 = (area2 != 0.0f) ? (1.0f / area2) : 0.0f;

        float minX = std::min({ tri.m_v[0].m_screenPos.x, tri.m_v[1].m_screenPos.x, tri.m_v[2].m_screenPos.x });
        float maxX = std::max({ tri.m_v[0].m_screenPos.x, tri.m_v[1].m_screenPos.x, tri.m_v[2].m_screenPos.x });
        float minY = std::min({ tri.m_v[0].m_screenPos.y, tri.m_v[1].m_screenPos.y, tri.m_v[2].m_screenPos.y });
        float maxY = std::max({ tri.m_v[0].m_screenPos.y, tri.m_v[1].m_screenPos.y, tri.m_v[2].m_screenPos.y });

        int xMin = static_cast<int>(std::floor(minX));
        int xMax = static_cast<int>(std::ceil(maxX));
        int yMin = static_cast<int>(std::floor(minY));
        int yMax = static_cast<int>(std::ceil(maxY));

        xMin = std::max(0, xMin);  xMax = std::min(static_cast<int>(WIDTH) - 1, xMax);
        yMin = std::max(0, yMin);  yMax = std::min(static_cast<int>(HEIGHT) - 1, yMax);

        tri.m_xMin = (xMin <= xMax) ? static_cast<uint32_t>(xMin) : 1;
        tri.m_xMax = (xMin <= xMax) ? static_cast<uint32_t>(xMax) : 0;
        tri.m_yMin = (yMin <= yMax) ? static_cast<uint32_t>(yMin) : 1;
        tri.m_yMax = (yMin <= yMax) ? static_cast<uint32_t>(yMax) : 0;

        float inv = tri.m_invArea2;
        glm::vec2& p0 = tri.m_v[0].m_screenPos;
        glm::vec2& p1 = tri.m_v[1].m_screenPos;
        glm::vec2& p2 = tri.m_v[2].m_screenPos;

        glm::vec2 e12 = p2 - p1;
        float A0 = -e12.y * inv;
        float B0 = e12.x * inv;
        float C0 = (e12.y * p1.x - e12.x * p1.y) * inv;

        glm::vec2 e20 = p0 - p2;
        float A1 = -e20.y * inv;
        float B1 = e20.x * inv;
        float C1 = (e20.y * p2.x - e20.x * p2.y) * inv;

        float A2 = -A0 - A1;
        float B2 = -B0 - B1;
        float C2 = 1.f - C0 - C1;

        tri.baryA = glm::vec2(A0, A1);
        tri.baryB = glm::vec2(B0, B1);
        tri.baryC = glm::vec2(C0, C1);

        float iw0 = tri.m_v[0].m_invW;
        float iw1 = tri.m_v[1].m_invW;
        float iw2 = tri.m_v[2].m_invW;

        glm::vec3 wp0 = tri.m_v[0].m_worldPos * iw0;
        glm::vec3 wp1 = tri.m_v[1].m_worldPos * iw1;
        glm::vec3 wp2 = tri.m_v[2].m_worldPos * iw2;

        glm::vec3 n0 = tri.m_v[0].m_normal * iw0;
        glm::vec3 n1 = tri.m_v[1].m_normal * iw1;
        glm::vec3 n2 = tri.m_v[2].m_normal * iw2;

        glm::vec2 uv0 = tri.m_v[0].m_uv * iw0;
        glm::vec2 uv1 = tri.m_v[1].m_uv * iw1;
        glm::vec2 uv2 = tri.m_v[2].m_uv * iw2;

        float As[3] = { A0, A1, A2 };
        float Bs[3] = { B0, B1, B2 };
        float Cs[3] = { C0, C1, C2 };

        tri.wordlPosA = glm::vec3(
            As[0] * wp0.x + As[1] * wp1.x + As[2] * wp2.x,
            As[0] * wp0.y + As[1] * wp1.y + As[2] * wp2.y,
            As[0] * wp0.z + As[1] * wp1.z + As[2] * wp2.z
        );
        tri.wordlPosB = glm::vec3(
            Bs[0] * wp0.x + Bs[1] * wp1.x + Bs[2] * wp2.x,
            Bs[0] * wp0.y + Bs[1] * wp1.y + Bs[2] * wp2.y,
            Bs[0] * wp0.z + Bs[1] * wp1.z + Bs[2] * wp2.z
        );
        tri.wordlPosC = glm::vec3(
            Cs[0] * wp0.x + Cs[1] * wp1.x + Cs[2] * wp2.x,
            Cs[0] * wp0.y + Cs[1] * wp1.y + Cs[2] * wp2.y,
            Cs[0] * wp0.z + Cs[1] * wp1.z + Cs[2] * wp2.z
        );

        tri.normalA = glm::vec3(
            As[0] * n0.x + As[1] * n1.x + As[2] * n2.x,
            As[0] * n0.y + As[1] * n1.y + As[2] * n2.y,
            As[0] * n0.z + As[1] * n1.z + As[2] * n2.z);
        tri.normalB = glm::vec3(
            Bs[0] * n0.x + Bs[1] * n1.x + Bs[2] * n2.x,
            Bs[0] * n0.y + Bs[1] * n1.y + Bs[2] * n2.y,
            Bs[0] * n0.z + Bs[1] * n1.z + Bs[2] * n2.z);
        tri.normalC = glm::vec3(
            Cs[0] * n0.x + Cs[1] * n1.x + Cs[2] * n2.x,
            Cs[0] * n0.y + Cs[1] * n1.y + Cs[2] * n2.y,
            Cs[0] * n0.z + Cs[1] * n1.z + Cs[2] * n2.z);

        tri.uvA = glm::vec2(As[0] * uv0.x + As[1] * uv1.x + As[2] * uv2.x, As[0] * uv0.y + As[1] * uv1.y + As[2] * uv2.y);
        tri.uvB = glm::vec2(Bs[0] * uv0.x + Bs[1] * uv1.x + Bs[2] * uv2.x, Bs[0] * uv0.y + Bs[1] * uv1.y + Bs[2] * uv2.y);
        tri.uvC = glm::vec2(Cs[0] * uv0.x + Cs[1] * uv1.x + Cs[2] * uv2.x, Cs[0] * uv0.y + Cs[1] * uv1.y + Cs[2] * uv2.y);

        tri.iwA = As[0] * iw0 + As[1] * iw1 + As[2] * iw2;
        tri.iwB = Bs[0] * iw0 + Bs[1] * iw1 + Bs[2] * iw2;
        tri.iwC = Cs[0] * iw0 + Cs[1] * iw1 + Cs[2] * iw2;
    }
}

void Rasterizer::Render(
    std::vector<RasterTriangle>& triangles,
    std::vector<std::unique_ptr<IPixelShader>>& pixelShaders,
    std::vector<Texture>& textures,
    std::vector<ConstantBuffer>& cbs,
    std::vector<DrawCommand>& drawCommands,
    FinalFrameBuffer& colorBuffer,
    std::vector<float>& depthBuffer)
{
    for (auto& tile : m_tileTriangles)
        tile.clear();

    for (uint32_t i = 0; i < triangles.size(); ++i)
    {
        RasterTriangle& tri = triangles[i];
        if (tri.m_invArea2 == 0.0f)
            continue;
        if (tri.m_xMin > tri.m_xMax || tri.m_yMin > tri.m_yMax)
            continue;

        uint32_t tx0 = tri.m_xMin / RASTER_CHUNK_SIZE;
        uint32_t tx1 = tri.m_xMax / RASTER_CHUNK_SIZE;
        uint32_t ty0 = tri.m_yMin / RASTER_CHUNK_SIZE;
        uint32_t ty1 = tri.m_yMax / RASTER_CHUNK_SIZE;

        for (uint32_t ty = ty0; ty <= ty1; ++ty)
            for (uint32_t tx = tx0; tx <= tx1; ++tx)
                m_tileTriangles[ty * CHUNKS_X + tx].push_back(i);
    }

    uint32_t nTiles = static_cast<uint32_t>(m_tileTriangles.size());

    #pragma omp parallel for schedule(guided)
    for (uint32_t t = 0; t < nTiles; ++t)
    {
        if (m_tileTriangles[t].empty())
            continue;

        uint32_t ty = uint32_t(t) / CHUNKS_X;
        uint32_t tx = uint32_t(t) % CHUNKS_X;

        uint32_t x0 = tx * RASTER_CHUNK_SIZE;
        uint32_t y0 = ty * RASTER_CHUNK_SIZE;
        uint32_t x1 = std::min(x0 + RASTER_CHUNK_SIZE, (uint32_t)WIDTH) - 1;
        uint32_t y1 = std::min(y0 + RASTER_CHUNK_SIZE, (uint32_t)HEIGHT) - 1;

        for (uint32_t triIdx : m_tileTriangles[t])
        {
            RasterTriangle& tri = triangles[triIdx];

            uint32_t xs = std::max(tri.m_xMin, x0);
            uint32_t xe = std::min(tri.m_xMax, x1);
            uint32_t ys = std::max(tri.m_yMin, y0);
            uint32_t ye = std::min(tri.m_yMax, y1);

            if (xs > xe || ys > ye)
                continue;   

            DrawCommand& dc = drawCommands[tri.m_drawCommandIndex];

            for (uint32_t y = ys; y <= ye; ++y)
                RasterizeLine(tri, y, xs, xe, depthBuffer, colorBuffer, *dc.m_pixelShader, dc.m_constantBuffer, dc.m_textureAlbedo, dc.m_cubeMap,dc.m_textureSpecular);
        }
    }
}

float Rasterizer::CalculateArea(glm::vec2 a, glm::vec2 b, glm::vec2 p)
{
    glm::vec2 ab = b - a;
    glm::vec2 ap = p - a;
    return ab.x * ap.y - ab.y * ap.x;
}

void Rasterizer::RasterizeLine(
    RasterTriangle& tri, uint32_t y,
    uint32_t xStart, uint32_t xEnd,
    std::vector<float>& depthBuffer,
    FinalFrameBuffer& colorBuffer,
    IPixelShader& ps, ConstantBuffer& cb,
    Texture* texAlbedo, CubeMap* cube,
    Texture* texSpecular

)
{
    float px0 = static_cast<float>(xStart) + 0.5f;  
    float py = static_cast<float>(y) + 0.5f;

    float bary0 = tri.baryA.x * px0 + tri.baryB.x * py + tri.baryC.x;
    float bary1 = tri.baryA.y * px0 + tri.baryB.y * py + tri.baryC.y;

    glm::vec3 wpNum = tri.wordlPosA * px0 + tri.wordlPosB * py + tri.wordlPosC;
    glm::vec3 nNum = tri.normalA * px0 + tri.normalB * py + tri.normalC;
    glm::vec2 uvNum = tri.uvA * px0 + tri.uvB * py + tri.uvC;
    float iwNum = tri.iwA * px0 + tri.iwB * py + tri.iwC;

    uint32_t rowBase = y * WIDTH;
    uint32_t triIdx = tri.m_v[0].m_triangleIndex;

    for (uint32_t x = xStart; x <= xEnd; ++x)
    {
        if (bary0 >= 0.000001f && bary1 >= 0.000001f && (bary0 + bary1) <= 1.000001f)
        {
            float& z = depthBuffer[rowBase + x];
            if (iwNum > z)
            {
                z = iwNum;
                float invWp = 1.0f / iwNum;
                glm::vec3 wp = wpNum * invWp;
                glm::vec3 n = nNum * invWp;
                glm::vec2 uv = uvNum * invWp;
                glm::vec2 p(static_cast<float>(x) + 0.5f, py);
                uint32_t color = ps.ComputePixel(wp, n, uv, p, triIdx, cb, texAlbedo, cube,texSpecular);
                colorBuffer.put_pixel(x, y, color);
            }
        }
        bary0 += tri.baryA.x; bary1 += tri.baryA.y;
        wpNum += tri.wordlPosA; nNum += tri.normalA;
        uvNum += tri.uvA; iwNum += tri.iwA;
    }
}