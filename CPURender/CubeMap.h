#pragma once
#include <string>
#include <glm/vec3.hpp>
#include "Texture.h"

class CubeMap
{
public:
	void LoadAllTextures(std::string paths[6]);
	void LoadTopTexture(std::string path);
	void LoadBottomTexture(std::string path);
	void LoadRightTexture(std::string path);
	void LoadLeftTexture(std::string path);
	void LoadFrontTexture(std::string path);
	void LoadBackTexture(std::string path);
	
	uint32_t Sample(glm::vec3 direction);
	
private:
	Texture m_top, m_bottom, m_right, m_left, m_front, m_back;
};