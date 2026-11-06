#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <bit>
#include <cmath>
#include "Utils.h"
#include <glm/common.hpp>

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::InitImageTexture(std::string path)
{
    int nbChanel;
    int width;
    int height;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nbChanel, 4);
    if (data == nullptr)
        return; //TODO raise err
    
    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);

    uint32_t textSize = GenerateTextureSize();
    m_textureData.resize(textSize * textSize);

    for (uint32_t x = 0; x < m_width; ++x)
    {
        for (uint32_t y = 0; y < m_height; ++y)
        {

            uint32_t colorValue = 0;
            uint32_t linearized2D = x + y * m_width;
            colorValue = colorValue | (uint32_t(data[0 + linearized2D * 4]) << 16);//First channel is Red, highest weight bits
            colorValue = colorValue | (uint32_t(data[1 + linearized2D * 4]) << 8); //Then green
            colorValue = colorValue | (uint32_t(data[2 + linearized2D * 4]) << 0); //Finally blue

            SetValueAt(colorValue, x, y);
        }
    }

    stbi_image_free(data);
}


std::vector<uint32_t>& Texture::GetTextureData()
{
    return m_textureData;
}

void Texture::SetTexture(std::vector<uint32_t>&newData)
{
    if (newData.size() != m_textureData.size())
        return; // can't upload different size textures
    std::copy(newData.begin(), newData.end(), m_textureData.begin());
}

void Texture::SetValueAt(uint32_t value, uint32_t x, uint32_t y)
{
    if (x >= m_width || y >= m_height)
        return; // raise err
    uint64_t mortonIndex = MortonEncode(x, y);
    m_textureData[mortonIndex] = value;
}

uint32_t Texture::Sample(float u, float v)
{
    u = u - std::floor(u);
    v = v - std::floor(v);

    float xf = u * m_width - 0.5f;
    float yf = (1.0f - v) * m_height - 0.5f;

    int   x0 = static_cast<int>(std::floor(xf));
    int   y0 = static_cast<int>(std::floor(yf));
    float fx = xf - std::floor(xf);  
    float fy = yf - std::floor(yf);

    int W = static_cast<int>(m_width);
    int H = static_cast<int>(m_height);

    uint32_t c00 = GetValueAt(Wrap(x0, W), Wrap(y0, H));
    uint32_t c10 = GetValueAt(Wrap(x0 + 1, W), Wrap(y0, H));
    uint32_t c01 = GetValueAt(Wrap(x0, W), Wrap(y0 + 1, H));
    uint32_t c11 = GetValueAt(Wrap(x0 + 1, W), Wrap(y0 + 1, H));

    glm::vec3 v00 = Utils::Uint32ToVec3(c00);
    glm::vec3 v10 = Utils::Uint32ToVec3(c10);
    glm::vec3 v01 = Utils::Uint32ToVec3(c01);
    glm::vec3 v11 = Utils::Uint32ToVec3(c11);

    glm::vec3 top = glm::mix(v00, v10, fx);
    glm::vec3 bottom = glm::mix(v01, v11, fx);
    glm::vec3 result = glm::mix(top, bottom, fy);

    return Utils::Vec3ToUint32(result);
}

uint32_t Texture::SampleClamp(float u, float v)
{
    float xf = u * m_width - 0.5f;
    float yf = (1.0f - v) * m_height - 0.5f;

    int x0 = static_cast<int>(std::floor(xf));
    int y0 = static_cast<int>(std::floor(yf));
    float fx = xf - std::floor(xf);
    float fy = yf - std::floor(yf);

    int W = static_cast<int>(m_width) - 1;
    int H = static_cast<int>(m_height) - 1;


    uint32_t c00 = GetValueAt(Clamp(x0, W), Clamp(y0, H));
    uint32_t c10 = GetValueAt(Clamp(x0 + 1, W), Clamp(y0, H));
    uint32_t c01 = GetValueAt(Clamp(x0, W), Clamp(y0 + 1, H));
    uint32_t c11 = GetValueAt(Clamp(x0 + 1, W), Clamp(y0 + 1, H));

    glm::vec3 v00 = Utils::Uint32ToVec3(c00);
    glm::vec3 v10 = Utils::Uint32ToVec3(c10);
    glm::vec3 v01 = Utils::Uint32ToVec3(c01);
    glm::vec3 v11 = Utils::Uint32ToVec3(c11);

    glm::vec3 top = glm::mix(v00, v10, fx);
    glm::vec3 bottom = glm::mix(v01, v11, fx);
    return Utils::Vec3ToUint32(glm::mix(top, bottom, fy));
}

uint32_t Texture::GetValueAt(uint32_t x, uint32_t y)
{
    if (x >= m_width || y >= m_height)
        return 0; // raise err
    return m_textureData[MortonEncode(x, y)];
}

int Texture::GetWidth()
{
    return m_width;
}

int Texture::GetHeight()
{
    return m_height;
}

uint32_t Texture::GenerateTextureSize()
{
    uint32_t max = std::max<uint32_t>(m_width, m_height);
    if (max == 0)
        return 1;
    max -= 1;
    int pow = std::bit_width(max);
    return static_cast<uint32_t>(std::pow(2,pow));
}

uint32_t Texture::Wrap(int i, int n)
{
    int r = i % n;
    return r < 0 ? r + n : r;
}

int Texture::Clamp(int i, int hi)
{
    return i < 0 ? 0 : (i > hi ? hi : i);
}

uint64_t Texture::MortonEncode(uint32_t x, uint32_t y) // https://stackoverflow.com/questions/30539347/2d-morton-code-encode-decode-64bits
{
    return _pdep_u32(x, 0x55555555) | _pdep_u32(y, 0xaaaaaaaa);
}
