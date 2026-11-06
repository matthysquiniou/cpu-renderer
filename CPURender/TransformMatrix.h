#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>

class TransformMatrix : public std::enable_shared_from_this<TransformMatrix> // need this shit to link parent and children correctly
{
public:
	TransformMatrix();
	~TransformMatrix();

	glm::vec3 GetLocalPosition();
	glm::vec3 GetWorldPosition();

	void TranslateLocal(const glm::vec3& tra);
	void TranslateWorld(const glm::vec3& tra);

	void SetPositionLocal(const glm::vec3& pos);
	void SetPositionWorld(const glm::vec3& pos);

	glm::quat GetRotationLocal();
	glm::quat GetRotationWorld();

	void SetRotationLocal(const glm::quat& rot);
	void SetRotationWorld(const glm::quat& rot);

	void RotateLocal(const glm::quat& rot);
	void RotateWorld(const glm::quat& rot);

	glm::vec3 GetLocalYPR();
	glm::vec3 GetWorldYPR();
	
	void SetLocalYPR(const glm::vec3& ypr);
	void SetWorldYPR(const glm::vec3& ypr);

	void RotateLocalYPR(const glm::vec3& ypr);
	void RotateWorldYPR(const glm::vec3& ypr);

	void LookAtLocal(const glm::vec3& at);
	void LookAtWorld(const glm::vec3& at);

	void LookToLocal(const glm::vec3& to);
	void LookToWorld(const glm::vec3& to);

	glm::vec3 GetForwardLocal();
	glm::vec3 GetForwardWorld();

	glm::vec3 GetUpLocal();
	glm::vec3 GetUpWorld();

	glm::vec3 GetScaleLocal();
	glm::vec3 GetScaleWorld();

	void SetScaleLocal(const glm::vec3& sca);
	void SetScaleWorld(const glm::vec3& sca);

	void ScaleByLocal(const glm::vec3& sca);
	void ScaleByWorld(const glm::vec3& sca);

	void ScaleFactorLocal(const float sca);
	void ScaleFactorWorld(const float sca);

	glm::mat4x4 GetLocalMatrix();
	glm::mat4x4 GetWorldMatrix();
	glm::mat4x4 GetInverseTransposeWorldMatrix();

	void SetParent(std::shared_ptr<TransformMatrix> parent);
	void SetChild(std::shared_ptr<TransformMatrix> child);

	void RemoveParent();
	void RemoveChild(std::shared_ptr<TransformMatrix> child);

	void MakeLocalDirty();
	void MakeWorldDirty();

private:
	void UpdateDirty();
	glm::mat4 Compose(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale);
	glm::quat QuatFromYPR(const glm::vec3& ypr);
	glm::vec3 YPRFromQuat(const glm::quat& quat);

private:
	static float s_epsilon;
	bool m_localDirty = false;
	bool m_worldDirty = false;

	glm::mat4x4 m_localMatrix = glm::mat4(1.0f);
	glm::mat4x4 m_worldMatrix = glm::mat4(1.0f);
	glm::mat4x4 m_invWorldTransposeMatrix = glm::mat4(1.0f);

	glm::vec3 m_localPosition = {0,0,0};
	glm::vec3 m_worldPosition = {0,0,0};

	glm::quat m_localRotation = {1,0,0,0};
	glm::quat m_worldRotation = {1,0,0,0};

	glm::vec3 m_localScale = { 1.f,1.f,1.f };
	glm::vec3 m_worldScale = { 1.f,1.f,1.f };

	std::vector<std::shared_ptr<TransformMatrix>> m_children;
	std::weak_ptr<TransformMatrix> m_parent;
};