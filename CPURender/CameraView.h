#pragma once
#include "Camera.h"

struct Plane {
    glm::vec3 n;
    float d;
};

class CameraView
{
public:
    CameraView();
    void UpdateViewProj();
    glm::mat4x4 GetMatViewProj();
    glm::mat4x4 GetMatView();
    glm::mat4x4 GetMatProj();
    Camera& GetCamera();
    TransformMatrix& GetTransform();
    std::vector<Plane> GetPlanes(glm::mat4x4 viewProjModel);

private:
    std::unique_ptr<Camera> m_mainCamera = nullptr;
    glm::mat4x4 m_view;
    glm::mat4x4 m_proj;
};

