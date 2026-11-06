#pragma once
#include <string>
#include <vector>
#include <stdint.h>

class Texture
{
public:
	Texture();
	~Texture();
	void InitImageTexture(std::string path);

	void SetTexture(std::vector<uint32_t>& newData);
	void SetValueAt(uint32_t value, uint32_t x, uint32_t y);
	uint32_t Sample(float u, float v);
	uint32_t SampleClamp(float u, float v);
	uint32_t GetValueAt(uint32_t x, uint32_t y);

	std::vector<uint32_t>& GetTextureData();
	int GetWidth();
	int GetHeight();
	uint64_t MortonEncode(uint32_t x, uint32_t y);


private:
	uint32_t GenerateTextureSize();
	uint32_t Wrap(int i, int n);
	int Clamp(int i, int hi);
public:
	float m_shininess = 8.f;
	float m_specularStrength = 0.1f;

private:
	uint32_t m_width;
	uint32_t m_height;
	std::vector<uint32_t> m_textureData;
};