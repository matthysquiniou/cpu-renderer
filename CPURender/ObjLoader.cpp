#include "ObjLoader.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<windows.h>
#include <glm/glm.hpp>
#include "Vertex.h"

struct VertexFace
{
	int posIndex = -1;
	int uvIndex = -1;
	int normalIndex = -1;
};

struct Face
{
	std::vector <VertexFace> vertexList;

	Face(std::vector <VertexFace> vertexL)
	{
		vertexList = vertexL;
	}
};
ObjLoader::ObjLoader()
{
	LoadAllAsset();
}

void ObjLoader::LoadAllAsset()
{
	//std::string assetFiles = "Assets/Obj/*";
}

void ObjLoader::LoadAsset(std::string path)
{
	std::ifstream file(path);
	std::string line;


	std::vector<glm::vec3> poslist;
	std::vector<glm::vec2> uvlist;
	std::vector<glm::vec3> normalList;
	std::vector<Face> faceList;
	std::vector <int> simplefaceList;
	Mesh m;
	int iteration = 0;

	while (std::getline(file, line))
	{
		std::istringstream readLine(line);
		std::string head = "";
		readLine >> head;

		if (head == "v")
		{
			glm::vec3 v;
			if (readLine >> v.x >> v.y >> v.z)
				poslist.push_back(v);
		}
		else if (head == "vt")
		{
			glm::vec2 uv;
			if (readLine >> uv.x >> uv.y)
				uvlist.push_back(uv);
		}
		else if (head == "vn")
		{
			glm::vec3 n;
			if (readLine >> n.x >> n.y >> n.z)
				normalList.push_back(n);
		}
		else if (head == "f")
		{
			std::string faceData;
			std::getline(readLine, faceData);

			std::replace(faceData.begin(), faceData.end(), '/', ' ');

			std::istringstream typeStream(faceData);

			std::vector<int> idxList;

			int idx;
			while (typeStream >> idx)
			{
				idxList.push_back(idx);
			}

			std::vector<VertexFace> vfList;
			for (int i = 0; i < idxList.size();i+=3)
			{
				VertexFace vf;
				vf.posIndex += idxList[i];
				vf.uvIndex += idxList[i+1];
				vf.normalIndex += idxList[i+2];

				vfList.push_back(vf);		
			}
			if (vfList.size() == 4)
			{
				Face f1(std::vector<VertexFace>{vfList[0], vfList[1], vfList[2]});
				faceList.push_back(f1);

				Face f2(std::vector<VertexFace>{vfList[0], vfList[2], vfList[3]});
				faceList.push_back(f2);
			}
			else if (vfList.size() == 3)
			{
				Face f1(std::vector<VertexFace>{vfList[0], vfList[1], vfList[2]});
				faceList.push_back(f1);
			}
		}
	}

	for (int i = 0; i < faceList.size();i++)
	{
		Vertex v;
		for (VertexFace vf : faceList[i].vertexList)
		{
			v.m_coordonate = poslist[vf.posIndex];
			v.m_normal = normalList[vf.normalIndex];
			v.m_uv = uvlist[vf.uvIndex];
			m.m_vertices.AddVertex(v);
		}
	
	}

	m.UpdateBBox();

	size_t posSlash = path.find_last_of("/\\");

	std::string name;
	if (posSlash != std::string::npos) {
		name = path.substr(posSlash + 1);
		size_t posPoint = name.find_last_of('.');
	}
	else {
		
		name = path; 
	}

	std::cout << name << std::endl;
	if (!m_meshBox.contains(name))
		m_meshBox[name] = m;
}

void ObjLoader::SetMesh(std::string name, Mesh& mesh)
{
	if (!m_meshBox.contains(name))
	{
		std::cout << "mesh : " << name << "isnt loaded ! " << std::endl;
		return;
	}
	mesh = m_meshBox[name];
}
