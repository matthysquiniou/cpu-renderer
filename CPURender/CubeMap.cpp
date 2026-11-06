#include "CubeMap.h"

void CubeMap::LoadAllTextures(std::string paths[6])
{
	m_top.InitImageTexture(paths[0]);
	m_bottom.InitImageTexture(paths[1]);
	m_right.InitImageTexture(paths[2]);
	m_left.InitImageTexture(paths[3]);
	m_front.InitImageTexture(paths[4]);
	m_back.InitImageTexture(paths[5]);
}

void CubeMap::LoadTopTexture(std::string path)
{
	m_top.InitImageTexture(path);
}

void CubeMap::LoadBottomTexture(std::string path)
{
	m_bottom.InitImageTexture(path);
}

void CubeMap::LoadRightTexture(std::string path)
{
	m_right.InitImageTexture(path);
}

void CubeMap::LoadLeftTexture(std::string path)
{
	m_left.InitImageTexture(path);
}

void CubeMap::LoadFrontTexture(std::string path)
{
	m_front.InitImageTexture(path);
}

void CubeMap::LoadBackTexture(std::string path)
{
	m_back.InitImageTexture(path);
}

uint32_t CubeMap::Sample(glm::vec3 direction)
{
    float x = fabsf(direction.x);
    float y = fabsf(direction.y);
    float z = fabsf(direction.z);

    if (x >= y && x >= z) {                 
        float inv = 0.5f / x;
        if (direction.x > 0.f) {            
            float u = 0.5f - direction.z * inv;
            float v = 0.5f + direction.y * inv;
            return m_right.SampleClamp(u, v);
        }
        else {                           
            float u = 0.5f + direction.z * inv;
            float v = 0.5f + direction.y * inv;
            return m_left.SampleClamp(u, v);
        }
    }
    else if (y >= z) {                     
        float inv = 0.5f / y;
        if (direction.y > 0.f) {         
            float u = 0.5f + direction.x * inv;
            float v = 0.5f - direction.z * inv;
            return m_top.SampleClamp(u, v);
        }
        else {                     
            float u = 0.5f + direction.x * inv;
            float v = 0.5f + direction.z * inv;
            return m_bottom.SampleClamp(u, v);
        }
    }
    else {                                 
        float inv = 0.5f / z;
        if (direction.z > 0.f) {         
            float u = 0.5f + direction.x * inv;
            float v = 0.5f + direction.y * inv;
            return m_front.SampleClamp(u, v);
        }
        else {                          
            float u = 0.5f - direction.x * inv;
            float v = 0.5f + direction.y * inv;
            return m_back.SampleClamp(u, v);
        }
    }
}