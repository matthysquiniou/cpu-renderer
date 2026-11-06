#pragma once
#include "Light.h"

class LightManager
{
public:
	static void SetAmbiantLight(AmbiantLight light);

	static uint32_t AddDirectionalLight(DirectionalLight light);
	static void DisableDirectionalLight(uint32_t lightID);

	static uint32_t AddPointLight(PointLight light);
	static void DisablePointLight(uint32_t lightID);

	static uint32_t AddSpotLight(SpotLight light);
	static void DisableSpotLight(uint32_t lightID);

	static LightsData* GetLightsData();
private:
	static uint32_t GetNewLightId();
	static void UpdateLightsDatas();

private:
	static AmbiantLight s_ambiantLight;

	static std::vector<DirectionalLight> s_directionalLights;
	static std::vector<PointLight> s_pointLights;
	static std::vector<SpotLight> s_spotLights;

	static LightsData s_lightsData;
	static bool s_lightsDataSet;

	static std::vector<uint32_t> s_disabledLights;
	static uint32_t s_nextLightId;

	static std::unordered_map<uint32_t, uint32_t> s_mapIndexDirectinalLights;
	static std::unordered_map<uint32_t, uint32_t> s_mapIndexPointLights;
	static std::unordered_map<uint32_t, uint32_t> s_mapIndexSpotLights;
};