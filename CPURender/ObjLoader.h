#pragma once
#include <unordered_map>
#include <string>
#include "Mesh.h"
class ObjLoader
{

private:
	static inline std::unordered_map<std::string, Mesh> m_meshBox;
public:

	ObjLoader();
	void LoadAllAsset();
	static void LoadAsset(std::string path);
	static void SetMesh(std::string name, Mesh& mesh);

};

