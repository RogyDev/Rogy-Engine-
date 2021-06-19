#ifndef R_TRANSFORM
#define R_TRANSFORM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

class RTransform 
{
public:
	RTransform *parent = nullptr;

	// mark the current node's tranform as dirty if it needs to be re-calculated this frame
	bool m_Dirty = true;

	glm::mat4 m_Transform = glm::mat4(1);
	glm::mat4 m_Orientation = glm::mat4(1);
	glm::vec3 Position    = glm::vec3(0, 0, 0);
	glm::vec3 Rotation    = glm::vec3(0, 0, 0);
	glm::vec3 Scale       = glm::vec3(1, 1, 1);

	glm::vec3 UpVec = glm::vec3(0, 0, 0);
	glm::vec3 RightVec = glm::vec3(0, 0, 0);
	glm::vec3 DirVec = glm::vec3(0, 0, 0);
	bool useLocalVecs = false;

	// Transformation
	// ---------------------
	void SetWorldPosition(glm::vec3 position);
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetScale(glm::vec3 scale);
	void SetScale(float scale);
	glm::vec3 GetLocalPosition();
	glm::vec3 GetLocalRotation();
	glm::vec3 GetLocalScale();
	glm::vec3 GetWorldPosition();
	glm::vec3 GetWorldScale();
	glm::mat4 GetTransform();

	glm::quat GetQuat();
	void SetQuat(float xx, float yy, float zz, float ww);

	void SetPosition(float xx, float yy, float zz);
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

	// Serialization
	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(Position.x, Position.y, Position.z);
		ar(Rotation.x, Rotation.y, Rotation.z);
		ar(Scale.x, Scale.y, Scale.z);
	}

	glm::vec3 _forw;
	glm::vec3 _up;
private:

};

#endif // R_TRANSFORM