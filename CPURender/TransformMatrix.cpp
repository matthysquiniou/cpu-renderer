#include "TransformMatrix.h"

float TransformMatrix::s_epsilon = 1e-7f;

TransformMatrix::TransformMatrix()
{
    
}

TransformMatrix::~TransformMatrix()
{
}

glm::vec3 TransformMatrix::GetLocalPosition()
{
    return m_localPosition;
}

glm::vec3 TransformMatrix::GetWorldPosition()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();

    return m_worldPosition;
}

void TransformMatrix::TranslateLocal(const glm::vec3& tra)
{
    m_localPosition += tra;
    MakeLocalDirty();
}

void TransformMatrix::TranslateWorld(const glm::vec3& tra)
{
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
    {
        glm::mat4 invParent = glm::inverse(parent->GetWorldMatrix());
        glm::vec4 localDelta = invParent * glm::vec4(tra, 0.0f);
        m_localPosition += glm::vec3(localDelta);
    }
    else
    {
        m_localPosition += tra;
    }
    MakeLocalDirty();
}

void TransformMatrix::SetPositionLocal(const glm::vec3& pos)
{
    m_localPosition = pos;
    MakeLocalDirty();
}

void TransformMatrix::SetPositionWorld(const glm::vec3& pos)
{
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
    {
        glm::mat4 invParent = glm::inverse(parent->GetWorldMatrix());
        glm::vec4 local = invParent * glm::vec4(pos, 1.0f);
        m_localPosition = glm::vec3(local) / local.w;
    }
    else
    {
        m_localPosition = pos;
    }
    MakeLocalDirty();
}

glm::quat TransformMatrix::GetRotationLocal()
{
    return m_localRotation;
}

glm::quat TransformMatrix::GetRotationWorld()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();

    return m_worldRotation;
}

void TransformMatrix::SetRotationLocal(const glm::quat& rot)
{
    m_localRotation = rot;
    MakeLocalDirty();
}

void TransformMatrix::SetRotationWorld(const glm::quat& rot)
{
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
    {
        glm::quat parentWorld = parent->GetRotationWorld();
        m_localRotation = glm::inverse(parentWorld) * rot;
    }
    else
    {
        m_localRotation = rot;
    }
    MakeLocalDirty();
}

void TransformMatrix::RotateLocal(const glm::quat& rot)
{
    m_localRotation = m_localRotation * rot;
    MakeLocalDirty();
}

void TransformMatrix::RotateWorld(const glm::quat& rot)
{
    glm::quat parentWorld;
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
        parentWorld = parent->GetRotationWorld();
    else
        parentWorld = glm::quat(1, 0, 0, 0);

    glm::quat oldWorld = parentWorld * m_localRotation;
    glm::quat newWorld = rot * oldWorld;
    m_localRotation = glm::inverse(parentWorld) * newWorld;
    MakeLocalDirty();
}

glm::vec3 TransformMatrix::GetLocalYPR()
{
    return YPRFromQuat(m_localRotation);
}

glm::vec3 TransformMatrix::GetWorldYPR()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();
    return YPRFromQuat(m_worldRotation);
}

void TransformMatrix::SetLocalYPR(const glm::vec3& ypr)
{
    m_localRotation = QuatFromYPR(ypr);
    MakeLocalDirty();
}

void TransformMatrix::SetWorldYPR(const glm::vec3& ypr)
{
    SetRotationWorld(QuatFromYPR(ypr));
}

void TransformMatrix::RotateLocalYPR(const glm::vec3& ypr)
{
    RotateLocal(QuatFromYPR(ypr));
}

void TransformMatrix::RotateWorldYPR(const glm::vec3& ypr)
{
    RotateWorld(QuatFromYPR(ypr));
}

void TransformMatrix::LookAtLocal(const glm::vec3& at)
{
    glm::vec3 dir = at - m_localPosition;
    if (glm::length(dir) < s_epsilon) 
        return;
    dir = glm::normalize(dir);
    if (std::abs(glm::dot(dir, glm::vec3(0, 1, 0))) > 0.9999f)
        m_localRotation = glm::quatLookAt(dir, glm::vec3(0, 0, 1));
    else
        m_localRotation = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    MakeLocalDirty();
}

void TransformMatrix::LookAtWorld(const glm::vec3& at)
{
    glm::vec3 dir = at - GetWorldPosition();
    if (glm::length(dir) < s_epsilon) 
        return;
    dir = glm::normalize(dir);
    glm::quat worldRot;
    if (std::abs(glm::dot(dir, glm::vec3(0, 1, 0))) > 0.9999f)
        worldRot = glm::quatLookAt(dir, glm::vec3(0, 0, 1));
    else
        worldRot = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    SetRotationWorld(worldRot);
}

void TransformMatrix::LookToLocal(const glm::vec3& to)
{
    if (glm::length(to) < s_epsilon) 
        return;
    glm::vec3 dir = glm::normalize(to);
    if (std::abs(glm::dot(dir, glm::vec3(0, 1, 0))) > 0.9999f)
        m_localRotation = glm::quatLookAt(dir, glm::vec3(0, 0, 1));
    else
        m_localRotation = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    MakeLocalDirty();
}

void TransformMatrix::LookToWorld(const glm::vec3& to)
{
    if (glm::length(to) < s_epsilon)
        return;
    glm::vec3 dir = glm::normalize(to);
    glm::quat worldRot;
    if (std::abs(glm::dot(dir, glm::vec3(0, 1, 0))) > 0.9999f)
        worldRot = glm::quatLookAt(dir, glm::vec3(0, 0, 1));
    else
        worldRot = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    SetRotationWorld(worldRot);
}

glm::vec3 TransformMatrix::GetForwardLocal()
{
    return glm::normalize(m_localRotation * glm::vec3(0, 0, -1));
}

glm::vec3 TransformMatrix::GetForwardWorld()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();
    return glm::normalize(m_worldRotation * glm::vec3(0, 0, -1));
}

glm::vec3 TransformMatrix::GetUpLocal()
{
    return glm::normalize(m_localRotation * glm::vec3(0, 1, 0));
}

glm::vec3 TransformMatrix::GetUpWorld()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();
    return glm::normalize(m_worldRotation * glm::vec3(0, 1, 0));
}

glm::vec3 TransformMatrix::GetScaleLocal()
{
    return m_localScale;
}

glm::vec3 TransformMatrix::GetScaleWorld()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();
    return m_worldScale;
}

void TransformMatrix::SetScaleLocal(const glm::vec3& sca)
{
    m_localScale = sca;
    MakeLocalDirty();
}

void TransformMatrix::SetScaleWorld(const glm::vec3& sca)
{
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
    {
        glm::vec3 parentScale = parent->GetScaleWorld();
        m_localScale = sca / parentScale;   
    }
    else
    {
        m_localScale = sca;
    }
    MakeLocalDirty();
}

void TransformMatrix::ScaleByLocal(const glm::vec3& sca)
{
    m_localScale *= sca;
    MakeLocalDirty();
}

void TransformMatrix::ScaleByWorld(const glm::vec3& sca)
{
    m_localScale *= sca;
    MakeLocalDirty();
}

void TransformMatrix::ScaleFactorLocal(float sca)
{
    m_localScale *= sca;
    MakeLocalDirty();
}

void TransformMatrix::ScaleFactorWorld(float sca)
{
    m_localScale *= sca;
    MakeLocalDirty();
}

glm::mat4x4 TransformMatrix::GetLocalMatrix()
{
    if (m_localDirty)
        UpdateDirty();
    return m_localMatrix;
}

glm::mat4x4 TransformMatrix::GetWorldMatrix()
{
    if (m_localDirty || m_worldDirty)
        UpdateDirty();
    return m_worldMatrix;
}

glm::mat4x4 TransformMatrix::GetInverseTransposeWorldMatrix()
{
    return m_invWorldTransposeMatrix;
}

void TransformMatrix::SetParent(std::shared_ptr<TransformMatrix> parent)
{
    RemoveParent();
    if (!parent || parent.get() == this)
        return;

    m_parent = parent;
    parent->m_children.push_back(shared_from_this());
    MakeWorldDirty();
}

void TransformMatrix::SetChild(std::shared_ptr<TransformMatrix> child)
{
    if (!child || child.get() == this)
        return;
    std::shared_ptr<TransformMatrix> oldParent = child->m_parent.lock();
    if (oldParent)
        oldParent->m_children.erase(std::remove(oldParent->m_children.begin(), oldParent->m_children.end(), child), oldParent->m_children.end());

    m_children.push_back(child);
    child->m_parent = shared_from_this();
    child->MakeWorldDirty();
}

void TransformMatrix::RemoveParent()
{
    std::shared_ptr<TransformMatrix> parent = m_parent.lock();
    if (parent)
    {
        parent->m_children.erase(std::remove_if(parent->m_children.begin(), parent->m_children.end(),
            [this](const std::shared_ptr<TransformMatrix>& c)
            { return c.get() == this; }
        ), parent->m_children.end());
    }
    m_parent.reset();
    MakeWorldDirty();
}

void TransformMatrix::RemoveChild(std::shared_ptr<TransformMatrix> child)
{
    if (!child)
        return;

    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());

    std::shared_ptr<TransformMatrix> parent = child->m_parent.lock();
    if (parent.get() == this)
        child->m_parent.reset();
    child->MakeWorldDirty();
}

void TransformMatrix::MakeLocalDirty()
{
    m_localDirty = true;
    MakeWorldDirty();
}

void TransformMatrix::MakeWorldDirty()
{
    m_worldDirty = true;
    for (size_t i = 0; i < m_children.size(); i++)
    {
        m_children[i].get()->MakeWorldDirty();
    }
}

void TransformMatrix::UpdateDirty()
{
    if (m_localDirty)
    {
        m_localMatrix = Compose(m_localPosition, m_localRotation, m_localScale);
        m_localDirty = false;
        m_worldDirty = true;
    }

    if (m_worldDirty)
    {
        std::shared_ptr<TransformMatrix> parent = m_parent.lock();
        if (parent != nullptr)
        {
            glm::mat4x4 parentWorld = parent->GetWorldMatrix();
            m_worldMatrix = parentWorld * m_localMatrix;
        }
        else
        {
            m_worldMatrix = m_localMatrix;
        }

        m_invWorldTransposeMatrix = glm::transpose(glm::inverse(m_worldMatrix));

        m_worldPosition = glm::vec3(m_worldMatrix[3]);

        glm::vec3 c0(m_worldMatrix[0]);
        glm::vec3 c1(m_worldMatrix[1]);
        glm::vec3 c2(m_worldMatrix[2]);
        m_worldScale = glm::vec3(glm::length(c0), glm::length(c1), glm::length(c2));

        glm::mat3 rotMat(c0 / m_worldScale.x, c1 / m_worldScale.y, c2 / m_worldScale.z);
        m_worldRotation = glm::normalize(glm::quat_cast(rotMat));

        m_worldDirty = false;
    }
}

glm::mat4 TransformMatrix::Compose(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, translation);
    m *= glm::mat4_cast(rotation);
    m = glm::scale(m, scale);
    return m;
}

glm::quat TransformMatrix::QuatFromYPR(const glm::vec3& ypr)
{
    glm::quat qy = glm::angleAxis(ypr.x, glm::vec3(0, 1, 0));
    glm::quat qx = glm::angleAxis(ypr.y, glm::vec3(1, 0, 0));
    glm::quat qz = glm::angleAxis(ypr.z, glm::vec3(0, 0, 1));
    return qy * qx * qz;
}

glm::vec3 TransformMatrix::YPRFromQuat(const glm::quat& quat)
{
    glm::mat3 R = glm::mat3_cast(quat);
    glm::vec3 ypr;
    float sx = glm::clamp(-R[2][1], -1.0f, 1.0f);
    ypr.y = std::asin(sx);                       
    if (std::abs(std::cos(ypr.y)) > s_epsilon) {
        ypr.x = std::atan2(R[2][0], R[2][2]);     
        ypr.z = std::atan2(R[0][1], R[1][1]);    
    }
    else {
        ypr.x = std::atan2(-R[0][2], R[0][0]);
        ypr.z = 0.0f;
    }
    return ypr;
}
