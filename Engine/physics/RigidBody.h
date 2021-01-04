#ifndef R_RIGIDBODY
#define R_RIGIDBODY

#include <btBulletDynamicsCommon.h>
#include "../scene/transform.h"
#include <string>
#include "../scene/RComponent.h"

enum RCollisionShapeType
{
	BOX_COLLIDER,
	SPHERE_COLLIDER,
	CAPSULE_COLLIDER,
	MESH_COLLIDER
};

enum RPhyBodyMode
{
	RB_RIGID,
	RB_STATIC,
	RB_CHARACTAR
};

class RigidBody : public Component
{
	BASE_COMPONENT()
public:
	RigidBody();
	~RigidBody();
	
	// ------------------------------------------------------------------------
	RCollisionShapeType m_CollisionType = BOX_COLLIDER;
	RPhyBodyMode		m_BodyMode = RB_RIGID;

	float Mass = 1.0f;
	float LinerDamping = 0.01f;
	float AngulerDamping = 0.0f;

	bool is_trigger = false;
	bool ListenForCollisions;
	
	glm::vec3 inertia = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 setedAngels;

	glm::vec3 mOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 mScaleMultiplier = glm::vec3(1.0f, 1.0f, 1.0f);;

	unsigned int id;
	int init_cf;

	// ------------------------------------------------------------------------
	btCollisionShape*   collisionShape;
	btRigidBody*        rigidBody;

	// ------------------------------------------------------------------------
	void SetCollisionShape(RCollisionShapeType col_type);
	void SetMass(float ms);
	void SetBodyMode(RPhyBodyMode btype);
	void SetDamping(float l_damp, float ang_damp);

	void SetPosition(glm::vec3& newPos);
	void SetAngels(glm::vec3& angles);
	void SetRotation(glm::quat& rotation);

	void ChangeShape(RCollisionShapeType toType);

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 vel);
	void Move(glm::vec3 vel);

	void AddForce(glm::vec3 force, glm::vec3 pos);
	void AddCentralForce(glm::vec3 force);

	void SetTrigger(bool is_trigg);
	// ------------------------------------------------------------------------
	btCollisionShape* GetCollisionShape();
	btRigidBody*	  GetRigidBody();
	btTransform&	  GetTransform();
	btMotionState*    GetMotionState();

	glm::vec3& GetScale();
	void SetCollisionScale(glm::vec3 newScale);
	void SetScale(glm::vec3 newScale);

	template<typename T>
	T* GetCollisionShapeT()
	{
		return static_cast<T*>(this->collisionShape);
	}

	// ------------------------------------------------------------------------

	// Serialization
	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar((int)m_CollisionType);
		ar((int)m_BodyMode);
		ar(Mass);
		ar(LinerDamping);
		ar(AngulerDamping);
		ar(is_trigger);
		ar(mScaleMultiplier.x, mScaleMultiplier.y, mScaleMultiplier.z);
		ar(mOffset.x, mOffset.y, mOffset.z);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		int coll_type, bdMode;
		float mss, l_damp, ang_damp;
		bool isTrigg;
		glm::vec3 last_scale;

		ar(coll_type);
		ar(bdMode);
		ar(mss);
		ar(l_damp);
		ar(ang_damp);
		ar(isTrigg);
		ar(last_scale.x, last_scale.y, last_scale.z);
		ar(mOffset.x, mOffset.y, mOffset.z);

		SetCollisionShape((RCollisionShapeType)coll_type);
		SetBodyMode((RPhyBodyMode)bdMode);
		SetMass(mss);
		SetDamping(l_damp, ang_damp);
		is_trigger = isTrigg;
		SetScale(last_scale);
		if (isTrigg)
			SetTrigger(is_trigger);
	}

private:

};


#endif // R_RIGIDBODY