#include "engine.h"
#include <stdlib.h>
#include <SDL3/SDL_events.h>
#include <iostream>
#include "Mesh.h"
#include "VertexShaderSimple.h"
#include "PixelShaderSimple.h"
#include "VertexShaderParticle.h"
#include "Cursor.h"
#include "LightManager.h"
#include "PixelShaderParticle.h"
#include "PixelShaderSkybox.h"
#include "PixelShaderProceduralMap.h"
#include "Logger.h"
#include "VertexShaderProceduralMap.h"

Engine::Engine() : m_particleManager(100'000)
{
}

void Engine::init()
{
    printf("Init engine \n");

    m_cameraView.UpdateViewProj();

    m_baseTextureHandle = m_renderAPI.CreateTexture("Assets/snow.jpg");
    m_texturesSpecularHandle = m_renderAPI.CreateSpecularTexture("Assets/wood_specular.png");
    m_fireTextureHandle = m_renderAPI.CreateTexture("Assets/fire_texture.jpg");

    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/water.jpg"));
    m_renderAPI.UploadTextureSpecular(m_texturesHandleProceduralMap.back(), 32.f, 0.7f);
    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/sand.jpg"));
    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/midgrass.jpg"));
    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/highgrass.jpg"));
    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/rock_base_color.png"));
    m_texturesHandleProceduralMap.push_back(m_renderAPI.CreateTexture("Assets/snow.jpg"));
    m_renderAPI.UploadTextureSpecular(m_texturesHandleProceduralMap.back(), 16.f, 1.f);

    std::string pathsCubeMapSkyBox[6] = { 
        "Assets/skybox_top.jpg", "Assets/skybox_bottom.jpg",
        "Assets/skybox_right.jpg", "Assets/skybox_left.jpg",
        "Assets/skybox_front.jpg", "Assets/skybox_back.jpg"
    };
    m_skyBoxCubeMapHandle = m_renderAPI.CreateCubeMap(pathsCubeMapSkyBox);

    m_vsSimple = m_renderAPI.CreateVertexShader<VertexShaderSimple>();
    m_vsProceduralMap = m_renderAPI.CreateVertexShader<VertexShaderProceduralMap>();
    m_vsParticle = m_renderAPI.CreateVertexShader<VertexShaderParticle>();

    m_psSimple = m_renderAPI.CreatePixelShader<PixelShaderSimple>();
    m_psProceduralMap = m_renderAPI.CreatePixelShader<PixelShaderProceduralMap>();
    m_psSkybox = m_renderAPI.CreatePixelShader<PixelShaderSkyBox>();
    m_psParticle = m_renderAPI.CreatePixelShader<PixelShaderParticle>();

    InitMesh();
    InitLight();
    InitParticles();
}

void Engine::InitMesh()
{
    m_meshes.push_back(Mesh());
    m_meshes[0].MakeCube();
    m_meshes[0].GetTransform().SetPositionWorld({ 0,40,0 });


    m_meshes.push_back(Mesh());
    ObjLoader::LoadAsset("Assets/girl.obj");
    ObjLoader::SetMesh("girl.obj", m_meshes[1]);
    m_meshes[1].GetTransform().SetPositionWorld({ 0,40,0 });
    m_meshes[1].GetTransform().SetScaleWorld({ 8,8,8 });
    m_meshes.push_back(Mesh());

    m_meshes.push_back(Mesh());
    ObjLoader::LoadAsset("Assets/sword.obj");
    ObjLoader::SetMesh("sword.obj", m_meshes[2]);
    m_meshes[2].GetTransform().SetPositionWorld({-50,70,0});
    m_meshes[2].GetTransform().SetScaleWorld({ 0.1f,0.1f,0.1f });



    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        Mesh& mesh = m_meshes[i];
        mesh.m_vertexBufferHandle = m_renderAPI.CreateVertexBuffer();
        m_renderAPI.UploadVertexBuffer(mesh.m_vertexBufferHandle, mesh.m_vertices.m_positions, mesh.m_vertices.m_normals, mesh.m_vertices.m_uvs);
        mesh.m_constantBufferHandle = m_renderAPI.CreateConstantBuffer();
        mesh.m_textureHandle = m_baseTextureHandle;
        mesh.m_vertexShaderHandle = m_vsSimple;
        mesh.m_pixelShaderHandle = m_psSimple;
    }

    m_perlinNoise.MakeProceduralMap();
    m_meshes.push_back(m_perlinNoise.GetProceduralMap());
    m_meshes.back().m_constantBufferHandle = m_renderAPI.CreateConstantBuffer();
    m_meshes.back().m_vertexBufferHandle = m_renderAPI.CreateVertexBuffer();
    m_meshes.back().m_vertexShaderHandle = m_vsProceduralMap;
    m_meshes.back().m_pixelShaderHandle = m_psProceduralMap;
    m_meshes.back().m_cubeMapHandle = m_skyBoxCubeMapHandle;
    m_renderAPI.UploadVertexBuffer(m_meshes.back().m_vertexBufferHandle, m_meshes.back().m_vertices.m_positions, m_meshes.back().m_vertices.m_normals, m_meshes.back().m_vertices.m_uvs);

    m_skyboxMesh.MakeInvertedCube(1000.f);
    m_skyboxMesh.m_constantBufferHandle = m_renderAPI.CreateConstantBuffer();
    m_skyboxMesh.m_vertexBufferHandle = m_renderAPI.CreateVertexBuffer();
    m_skyboxMesh.m_vertexShaderHandle = m_vsSimple;
    m_skyboxMesh.m_pixelShaderHandle = m_psSkybox;
    m_skyboxMesh.m_cubeMapHandle = m_skyBoxCubeMapHandle;
    m_renderAPI.UploadVertexBuffer(m_skyboxMesh.m_vertexBufferHandle, m_skyboxMesh.m_vertices.m_positions, m_skyboxMesh.m_vertices.m_normals, m_skyboxMesh.m_vertices.m_uvs);
    m_meshes.push_back(m_skyboxMesh);
}

void Engine::InitLight()
{
    LightManager::SetAmbiantLight({});
    LightManager::AddDirectionalLight({ glm::vec3{1,1,1}, glm::vec3{0.5f,-1.f,0} });
    // LightManager::AddDirectionalLight({ glm::vec3{0,0,1}, glm::vec3{0,1,0} });
    // LightManager::AddDirectionalLight({ glm::vec3{0,1,0}, glm::vec3{0,-1,0} });
    // LightManager::AddDirectionalLight({ glm::vec3{0,1,1}, glm::vec3{1,0,0} });
    // LightManager::AddDirectionalLight({ glm::vec3{1,0,0}, glm::vec3{-1,0,0} });
    // LightManager::AddDirectionalLight({ glm::vec3{1,0,1}, glm::vec3{0,0,1} });
    // LightManager::AddDirectionalLight({ glm::vec3{1,1,0}, glm::vec3{0,0,-1} });
}

void Engine::InitParticles()
{
    m_particleManager.AddParticleEmitor(ParticleEmitor::CreateSmokeEmitor({ 5, 0, 5 }));
    m_particleManager.AddParticleEmitor(ParticleEmitor::CreateFireFlameEmitor({ 5, 0, 5 }));
    m_particleManager.AddParticleEmitor(ParticleEmitor::CreateFireCoreEmitor({ 5, 0, 5 }));
    m_particleManager.AddParticleEmitor(ParticleEmitor::CreateRainEmitor({0,0,0}));
    m_particleManager.AddParticleEmitor(ParticleEmitor::CreateSparkEmitor({-5,0,-5}));

    Mesh& particleMesh = m_particleManager.GetParticleMesh();
    particleMesh.m_constantBufferHandle = m_renderAPI.CreateConstantBuffer();
    particleMesh.m_vertexBufferHandle = m_renderAPI.CreateVertexBuffer();
    particleMesh.m_vertexShaderHandle = m_vsParticle;
    particleMesh.m_pixelShaderHandle = m_psParticle;
    m_renderAPI.UploadVertexBuffer(particleMesh.m_vertexBufferHandle, particleMesh.m_vertices.m_positions, particleMesh.m_vertices.m_normals, particleMesh.m_vertices.m_uvs);
}

void Engine::handleEvent(const SDL_Event& event)
{
    Logger::HandleEvent(event);

    float dt = m_clock.GetDt();
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        switch (event.key.key)
        {
        case SDLK_SPACE:
            m_cameraView.GetCamera().MoveUp(m_cameraSpeed * dt);
            break;
        case SDLK_LSHIFT:
            m_cameraView.GetCamera().MoveDown(m_cameraSpeed * dt);
            break;
        case SDLK_W:
        case SDLK_Z:
            m_cameraView.GetCamera().MoveForward(m_cameraSpeed * dt);
            break;
        case SDLK_A:
        case SDLK_Q:
            m_cameraView.GetCamera().MoveLeft(m_cameraSpeed * dt);
            break;
        case SDLK_S:
            m_cameraView.GetCamera().MoveBackward(m_cameraSpeed * dt);
            break;
        case SDLK_D:
            m_cameraView.GetCamera().MoveRight(m_cameraSpeed * dt);
            break;
        case SDLK_C:
            m_backfaceCullingClockwise = !m_backfaceCullingClockwise;
            break;
        case SDLK_M:
            m_canShowPerlinNoise = !m_canShowPerlinNoise;
            break;
        case SDLK_ESCAPE:
            ToggleMouseLock();
            break;
        default:
            break;
        }
    }
}

void Engine::ToggleMouseLock()
{
    Cursor::SetCursorVisible(m_mouseLocked);
    Cursor::ResetCursorDelta();
    m_mouseLocked = !m_mouseLocked;
    if (m_mouseLocked)
    {
        Cursor::SetCursorPositionCenter();
        Cursor::ResetCursorDelta();
    }
}

void Engine::UpdateMouse()
{
    if (m_mouseLocked)
    {
        float dt = m_clock.GetDt();

        glm::vec2 mouseDelta = Cursor::GetCursorDelta();
        Cursor::SetCursorPositionCenter();
        Cursor::ResetCursorDelta();
        m_cameraView.GetCamera().RotateUpAxis(mouseDelta.x * dt * m_mouseSensitivity);
        m_cameraView.GetCamera().RotateRightAxis(mouseDelta.y * dt * m_mouseSensitivity);
    }
}

void Engine::Update()
{
    m_clock.UpdateDt();
    m_particleManager.UpdateParticles(m_clock.GetDt());
    UpdateMouse();
    m_cameraView.UpdateViewProj();

    std::vector<size_t> meshesIndexToDraw;
    meshesIndexToDraw.reserve(m_meshes.size());

    //m_meshes[0].GetTransform().RotateLocalYPR({0,0.01f,0.02f});

    for (size_t meshIndex = 0; meshIndex < m_meshes.size(); meshIndex++)
    {
        Mesh& mesh = m_meshes[meshIndex];
        glm::mat4x4 mv = m_cameraView.GetMatViewProj() * mesh.GetTransform().GetWorldMatrix();
        std::vector<Plane> planes = m_cameraView.GetPlanes(mv);
    
        bool outside = false;
        for (int planeIndex = 0; planeIndex < 6 && !outside; ++planeIndex)
        {
            bool anyInside = false;
            for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
            {
                glm::vec3& corner = mesh.m_BBoxCorners[cornerIndex];
                float dist = planes[planeIndex].n.x * corner.x + planes[planeIndex].n.y * corner.y + planes[planeIndex].n.z * corner.z + planes[planeIndex].d;
    
                if (dist >= 0.f)
                { 
                    anyInside = true;
                    break;
                }
            }
            if (!anyInside)
                outside = true;
        }
    
        if (outside)
            continue;
    
        meshesIndexToDraw.push_back(meshIndex);
    }
    
    for (size_t i = 0; i < meshesIndexToDraw.size(); i++)
    {
        size_t meshIndex = meshesIndexToDraw[i];
        Mesh& mesh = m_meshes[meshIndex];
        ConstantBuffer cb;
        cb.m_viewProj = m_cameraView.GetMatViewProj();
        cb.m_model = mesh.m_transform->GetWorldMatrix();
        cb.m_InvTransposeModel = mesh.m_transform->GetInverseTransposeWorldMatrix();
        cb.m_cameraPos = m_cameraView.GetTransform().GetWorldPosition();
        cb.m_lightsData = LightManager::GetLightsData();
        cb.m_TexturesIndices = m_texturesHandleProceduralMap;
        cb.m_deltaTime = m_clock.GetDt();
        cb.m_totalTime = m_clock.GetTotalTime();
        m_renderAPI.UploadConstantBuffer(mesh.m_constantBufferHandle, cb);
        m_renderAPI.AddVertexBufferToDraw(mesh.m_vertexBufferHandle, mesh.m_constantBufferHandle, mesh.m_vertexShaderHandle, mesh.m_pixelShaderHandle, mesh.m_textureHandle, mesh.m_cubeMapHandle, -1, m_backfaceCullingClockwise);
    }
    
    // particles
    if (m_particleManager.GetAliveParticleNumber() != 0)
    {
        Mesh& mesh = m_particleManager.GetParticleMesh();
        ConstantBuffer cb;
        cb.m_view = m_cameraView.GetMatView();
        cb.m_proj = m_cameraView.GetMatProj();
        cb.m_cameraPos = m_cameraView.GetTransform().GetWorldPosition();
        cb.m_lightsData = LightManager::GetLightsData();
        cb.m_particleAlive = m_particleManager.GetAliveParticleNumber();
        cb.m_particles = m_particleManager.GetParticlesData();
        cb.m_deltaTime = m_clock.GetDt();
        cb.m_totalTime = m_clock.GetTotalTime();
        m_renderAPI.UploadConstantBuffer(mesh.m_constantBufferHandle, cb);
        m_renderAPI.AddVertexBufferToDraw(mesh.m_vertexBufferHandle, mesh.m_constantBufferHandle, mesh.m_vertexShaderHandle, mesh.m_pixelShaderHandle);
    }
}

void Engine::renderFrame(FinalFrameBuffer& frameBufferOut)
{
    frameBufferOut.clear(0x000000);
    m_renderAPI.FlushDraw(frameBufferOut);
    if (m_canShowPerlinNoise)
        m_perlinNoise.RenderPerlinNoise(frameBufferOut);
    Logger::ResetKey();
}

void Engine::destroy()
{
    printf("Killing engine \n");
}
