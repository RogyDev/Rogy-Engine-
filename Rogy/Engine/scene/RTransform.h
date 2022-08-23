#ifndef RR_TRANSFORM_H
#define RR_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include "RComponent.h"
#include <yaml-cpp/yaml.h>

class Transform 
{
public:

	Transform();
	~Transform();

	Transform *parent = nullptr;

	// mark the current node's transform as dirty if it needs to be re-calculated this frame
	bool m_Dirty = true;
	bool transformation_changed = false;

	glm::mat4 m_Transform = glm::mat4(1);

	glm::mat4 m_Last_Transform = glm::mat4(1);
	glm::mat4 m_Orientation = glm::mat4(1);

	glm::vec3 Position = glm::vec3(0, 0, 0);
	glm::quat Rotation = glm::quat();
	glm::vec3 Scale = glm::vec3(1, 1, 1);

	glm::vec3 Angels = glm::vec3(0, 0, 0);

	glm::vec3 WPosition = glm::vec3(0, 0, 0);
	glm::quat WRotation = glm::quat();
	glm::vec3 WScale = glm::vec3(1, 1, 1);

	bool noApply = false;

	// Transformation
	// ---------------------
	void SetMat4(glm::mat4 sMat4);
	void SetWorldPosition(glm::vec3 position);
	void SetWorldScale(glm::vec3 sca);
	void SetLocalScale(glm::vec3 sca);
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::quat rotation);
	void SetScale(glm::vec3 scale);
	void SetScaleA(float scale);
	glm::vec3 GetLocalPosition();
	glm::quat GetLocalRotation();
	glm::vec3 GetLocalScale();
	glm::vec3 GetWorldPosition();
	glm::quat GetWorldRotation();
	glm::vec3 GetWorldScale();
	glm::mat4 GetTransform();

	glm::vec3 GetEurlerAngelsRad();
	glm::vec3 GetEurlerAngels();

	void SetAngels(glm::vec3 angels);
	void LookAt(glm::vec3 target);

	void Translate(glm::vec3 dir);

	//void SetPosition(float xx, float yy, float zz);
	void SetRotation(float xx, float yy, float zz);
	void SetScale(float xx, float yy, float zz);

	void UpdateTransform();

	// Transform vectors
	// -----------------------
	glm::vec3 direction();
	glm::vec3	 right();
	glm::vec3	    up();
	glm::vec3   forward();
	float  Deg2Rad();
	bool saving = false;

	// Serialization
	static glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	static glm::quat RLookAt(glm::vec3 direction, glm::vec3 desiredUp);
	static void SerVec3(YAML::Emitter& out, const glm::vec3& v);
	static void SerQuat(YAML::Emitter& out, const glm::quat& v);
	static glm::vec3 GetVec3(YAML::Node& out);
	static glm::quat GetQuat(YAML::Node& out);
	void SerializeSave(YAML::Emitter& out);
	void SerializeLoad(YAML::Node& out);

	glm::vec3 _forw;
	glm::vec3 _up;
private:

};


#endif // R_TRANSFORM