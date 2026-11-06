#include "LightManager.h"

AmbiantLight LightManager::s_ambiantLight;

std::vector<DirectionalLight> LightManager::s_directionalLights;
std::vector<PointLight> LightManager::s_pointLights;
std::vector<SpotLight> LightManager::s_spotLights;

std::vector<uint32_t> LightManager::s_disabledLights;
uint32_t LightManager::s_nextLightId = 0;

LightsData LightManager::s_lightsData;
bool LightManager::s_lightsDataSet = false;

std::unordered_map<uint32_t, uint32_t> LightManager::s_mapIndexDirectinalLights;
std::unordered_map<uint32_t, uint32_t> LightManager::s_mapIndexPointLights;
std::unordered_map<uint32_t, uint32_t> LightManager::s_mapIndexSpotLights;

void LightManager::SetAmbiantLight(AmbiantLight light)
{
	s_ambiantLight = light;
}

uint32_t LightManager::AddDirectionalLight(DirectionalLight light)
{
	uint32_t id = GetNewLightId();

	s_mapIndexDirectinalLights[id] = static_cast<uint32_t>(s_directionalLights.size());
	s_directionalLights.push_back(light);

	return id;
}

void LightManager::DisableDirectionalLight(uint32_t lightID)
{
	auto it = s_mapIndexDirectinalLights.find(lightID);
	if (it == s_mapIndexDirectinalLights.end())
		return; 

	uint32_t index = it->second;
	uint32_t lastIndex = static_cast<uint32_t>(s_directionalLights.size()) - 1;

	if (index != lastIndex)
	{
		s_directionalLights[index] = std::move(s_directionalLights[lastIndex]);

		for (auto& [id, idx] : s_mapIndexDirectinalLights)
		{
			if (idx == lastIndex)
			{
				idx = index;
				break;
			}
		}
	}

	s_directionalLights.pop_back();
	s_mapIndexDirectinalLights.erase(it);
	s_disabledLights.push_back(lightID);
}

uint32_t LightManager::AddPointLight(PointLight light)
{
	uint32_t id = GetNewLightId();

	s_mapIndexPointLights[id] = static_cast<uint32_t>(s_pointLights.size());
	s_pointLights.push_back(light);

	return id;
}

void LightManager::DisablePointLight(uint32_t lightID)
{
	auto it = s_mapIndexPointLights.find(lightID);
	if (it == s_mapIndexPointLights.end())
		return;

	uint32_t index = it->second;
	uint32_t lastIndex = static_cast<uint32_t>(s_pointLights.size()) - 1;

	if (index != lastIndex)
	{
		s_pointLights[index] = std::move(s_pointLights[lastIndex]);

		for (auto& [id, idx] : s_mapIndexPointLights)
		{
			if (idx == lastIndex)
			{
				idx = index;
				break;
			}
		}
	}

	s_pointLights.pop_back();
	s_mapIndexPointLights.erase(it);
	s_disabledLights.push_back(lightID);
}

uint32_t LightManager::AddSpotLight(SpotLight light)
{
	uint32_t id = GetNewLightId();

	light.m_invRange = 1 / light.m_range;

	s_mapIndexSpotLights[id] = static_cast<uint32_t>(s_spotLights.size());
	s_spotLights.push_back(light);

	return id;
}

void LightManager::DisableSpotLight(uint32_t lightID)
{
	auto it = s_mapIndexSpotLights.find(lightID);
	if (it == s_mapIndexSpotLights.end())
		return;

	uint32_t index = it->second;
	uint32_t lastIndex = static_cast<uint32_t>(s_spotLights.size()) - 1;

	if (index != lastIndex)
	{
		s_spotLights[index] = std::move(s_spotLights[lastIndex]);

		for (auto& [id, idx] : s_mapIndexSpotLights)
		{
			if (idx == lastIndex)
			{
				idx = index;
				break;
			}
		}
	}

	s_spotLights.pop_back();
	s_mapIndexSpotLights.erase(it);
	s_disabledLights.push_back(lightID);
}

LightsData* LightManager::GetLightsData()
{
	if (!s_lightsDataSet)
		UpdateLightsDatas();
	return &s_lightsData;
}

uint32_t LightManager::GetNewLightId()
{
	uint32_t id;
	if (!s_disabledLights.empty())
	{
		id = s_disabledLights.back();
		s_disabledLights.pop_back();
	}
	else
	{
		id = static_cast<uint32_t>(s_nextLightId++);
	}
	return id;
}

void LightManager::UpdateLightsDatas()
{
	s_lightsData.m_ambiantLight = &s_ambiantLight;
	s_lightsData.m_directionalLights = &s_directionalLights;
	s_lightsData.m_pointLights = &s_pointLights;
	s_lightsData.m_spotLights = &s_spotLights;
	s_lightsDataSet = true;
}
