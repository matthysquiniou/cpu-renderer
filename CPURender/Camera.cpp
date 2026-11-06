#include "Camera.h"

void Camera::MoveForward(float distance)
{
    m_transform->TranslateWorld(m_transform->GetForwardWorld() * distance);
}

void Camera::MoveBackward(float distance)
{
    m_transform->TranslateWorld(-m_transform->GetForwardWorld() * distance);
}

void Camera::MoveUp(float distance)
{
    m_transform->TranslateWorld(m_transform->GetUpWorld() * distance);
}

void Camera::MoveDown(float distance)
{
    m_transform->TranslateWorld(-m_transform->GetUpWorld() * distance);
}

void Camera::MoveRight(float distance)
{
    glm::vec3 forward = m_transform->GetForwardWorld();
    glm::vec3 up = m_transform->GetUpWorld();
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    m_transform->TranslateWorld(right * distance);
}

void Camera::MoveLeft(float distance)
{
    glm::vec3 forward = m_transform->GetForwardWorld();
    glm::vec3 up = m_transform->GetUpWorld();
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    m_transform->TranslateWorld(-right * distance);
}

void Camera::RotateUpAxis(float radian)
{
	glm::quat q = glm::angleAxis(radian, glm::vec3(0.f, -1.f, 0.f));
	m_transform->RotateWorld(q);
}

void Camera::RotateRightAxis(float radian)
{
	glm::quat q = glm::angleAxis(radian, glm::vec3(-1.f, 0.f, 0.f));
	m_transform->RotateLocal(q);
}
