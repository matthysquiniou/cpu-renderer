#pragma once
#include "main.h"
#include <memory>
#include "RenderAPI.h"
#include "Mesh.h"
#include "CameraView.h"
#include "ObjLoader.h"
#include "Clock.h"
#include "PerlinNoise.h"
#include "ParticleManager.h"
#include "CubeMap.h"

union SDL_Event;

struct Engine {
	Engine();

	void init();
	void InitMesh();
	void InitLight();
	void InitParticles();

	void handleEvent(const SDL_Event& event);
	void ToggleMouseLock();
	void UpdateMouse();

	void Update();
	
	void renderFrame(FinalFrameBuffer& frameBufferOut);
	void destroy();

	static ObjLoader m_obj;
	CameraView m_cameraView;
	float m_cameraSpeed = 30.f;
	float m_mouseSensitivity = 0.1f;
	bool m_mouseLocked = false;
	bool m_canShowPerlinNoise = false;
	PerlinNoise m_perlinNoise;
	Clock m_clock;
	RenderAPI m_renderAPI;
	ParticleManager m_particleManager;

	std::vector<Mesh> m_meshes;
	Mesh m_skyboxMesh;

	uint32_t m_texturesSpecularHandle;

	uint32_t m_fireTextureHandle;
	uint32_t m_baseTextureHandle;
	std::vector<uint32_t> m_texturesHandleProceduralMap;
	uint32_t m_skyBoxCubeMapHandle = 0;

	uint32_t m_vsSimple;
	uint32_t m_vsProceduralMap;
	uint32_t m_vsParticle;

	uint32_t m_psSimple;
	uint32_t m_psProceduralMap;
	uint32_t m_psParticle;
	uint32_t m_psSkybox;

	bool m_backfaceCullingClockwise = false;
};