#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "main.h"

#define MINHEIGHT -45
#define MIDHEIGHT -20
#define HIGHHEIGHT 25
#define MAXHEIGHT 50

#define WIDTHMAP 256
#define HEIGHTMAP 256


class PerlinNoise
{
private:
	std::vector<int> m_permutation;
	std::vector<std::vector<glm::vec3>> m_heightMap;
	std::vector<glm::vec3> m_pixelToDraw;
	Mesh m_mesh;
private:
	void Shuffle();
	void MakePermutation();
	int Color(glm::vec3 rgb);
	glm::vec2 GetConstantVector(int value);
	float Fade(float t);
	float Lerp(float t, float a1, float a2);
	float Noise2D(float x, float y);
	float FractalBrownianMotion(float x, float y, int numOctaves);
	glm::vec3 CreateNormal(int col, int row);
	void CreatePlaneVertex(int col, int row);
	void CreatePerlinNoise();
	int GetFieldColor(float t);

public:
	PerlinNoise();
	Mesh& GetProceduralMap();
	void MakeProceduralMap();
	void RenderPerlinNoise(FinalFrameBuffer& frameBuffer);
};

