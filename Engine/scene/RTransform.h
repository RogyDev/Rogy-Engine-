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
	template <class Archive>
	void serialize(Archive & ar)
	{
		if (noApply)
		{
			glm::vec3 Posi = glm::vec3(0, 0, 0);
			//glm::vec3 Rota = glm::vec3(0, 0, 0);
			glm::quat Rota = glm::quat();
			glm::vec3 Sca = glm::vec3(1, 1, 1);
			ar(Posi.x, Posi.y, Posi.z);
			ar(Rota.x, Rota.y, Rota.z, Rota.w);
			ar(Sca.x, Sca.y, Sca.z);
			noApply = false;
		}
		else
		{
			ar(Position.x, Position.y, Position.z);
			ar(Rotation.x, Rotation.y, Rotation.z, Rotation.w);
			//ar(Angels.x, Angels.y, Angels.z);
			Angels = glm::degrees(glm::eulerAngles(Rotation));
			ar(Scale.x, Scale.y, Scale.z);
		}
	}

	glm::vec3 _forw;
	glm::vec3 _up;
private:

};


#endif // R_TRANSFORM