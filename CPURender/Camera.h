#pragma once
#include "TransformMatrix.h"
#include "main.h"

struct Camera
{
    std::shared_ptr<TransformMatrix> m_transform;

    float m_aspect = (float)WIDTH / (float)HEIGHT;
    float m_near = 0.1f;
    float m_far = 2500.f;
    float m_fov = glm::radians(45.0f);

    void MoveUp(float distance);
    void MoveDown(float distance);
    void MoveForward(float distance);
    void MoveBackward(float distance);
    void MoveLeft(float distance);
    void MoveRight(float distance);
    void RotateUpAxis(float radian);
    void RotateRightAxis(float radian);
};
