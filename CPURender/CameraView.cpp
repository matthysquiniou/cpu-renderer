#include "CameraView.h"

CameraView::CameraView()
{
	if (m_mainCamera == nullptr)
	{
		m_mainCamera = std::make_unique<Camera>();
		m_mainCamera->m_transform = std::make_shared<TransformMatrix>();
		m_mainCamera->m_transform->SetPositionWorld({ 0,0,5 });
	}
}

void CameraView::UpdateViewProj()
{
	m_view = glm::lookAt(m_mainCamera->m_transform->GetWorldPosition(), m_mainCamera->m_transform->GetForwardWorld() + m_mainCamera->m_transform->GetWorldPosition(), m_mainCamera->m_transform->GetUpWorld());
	m_proj = glm::mat4x4{ glm::perspective(m_mainCamera->m_fov, m_mainCamera->m_aspect, m_mainCamera->m_near, m_mainCamera->m_far) };
}

glm::mat4x4 CameraView::GetMatViewProj()
{
	return m_proj * m_view;
}

glm::mat4x4 CameraView::GetMatView()
{
    return m_view;
}

glm::mat4x4 CameraView::GetMatProj()
{
    return m_proj;
}

Camera& CameraView::GetCamera()
{
	return *m_mainCamera.get();
}

TransformMatrix& CameraView::GetTransform()
{
	return *m_mainCamera.get()->m_transform;
}

std::vector<Plane> CameraView::GetPlanes(glm::mat4x4 viewProjModel)
{
    std::vector<Plane> planes;
    planes.resize(6);
    glm::vec4 r0(viewProjModel[0][0], viewProjModel[1][0], viewProjModel[2][0], viewProjModel[3][0]);
    glm::vec4 r1(viewProjModel[0][1], viewProjModel[1][1], viewProjModel[2][1], viewProjModel[3][1]);
    glm::vec4 r2(viewProjModel[0][2], viewProjModel[1][2], viewProjModel[2][2], viewProjModel[3][2]);
    glm::vec4 r3(viewProjModel[0][3], viewProjModel[1][3], viewProjModel[2][3], viewProjModel[3][3]);

    glm::vec4 e[6] = { r3 + r0, r3 - r0, r3 + r1, r3 - r1, r3 + r2, r3 - r2 };

    for (int i = 0; i < 6; ++i)
    {
        planes[i] = { glm::vec3(e[i]), e[i].w };
    }

    return planes;
}