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
	RB_CHARACTAR,
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
	float Friction = 1.0f;
	float Bounciness = 0.0f;

	bool is_trigger = false;
	bool is_kinematic = false;
	bool ListenForCollisions;
	glm::vec3 Gravity = glm::vec3(0.0f, -1.0f, 0.0f);;
	
	glm::vec3 inertia = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 setedAngels;

	glm::vec3 mOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 mScaleMultiplier = glm::vec3(1.0f, 1.0f, 1.0f);;

	unsigned int id;
	int init_cf;
	bool smfs = false;
	// ------------------------------------------------------------------------
	btCollisionShape*   collisionShape;
	btRigidBody*        rigidBody;
	std::string mesh_path;
	int mesh_index;

	// ------------------------------------------------------------------------
	void SetCollisionShape(RCollisionShapeType col_type);
	void SetCollisionMesh(btTriangleMesh* tm, std::string& meshPath, int indx);
	void SetMass(float ms);
	void SetBodyMode(RPhyBodyMode btype);
	void SetDamping(float l_damp, float ang_damp);

	void SetPosition(glm::vec3& newPos);
	glm::vec3 GetPosition();
	void SetAngels(glm::vec3& angles);
	void SetRotation(glm::quat& rotation);

	void ChangeShape(RCollisionShapeType toType);

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 vel);
	void Move(glm::vec3 vel);

	void SetGravityY(float g);
	void SetGravity(glm::vec3 g);
	glm::vec3 GetGravity();

	void SetBounciness(float v);
	float GetBounciness();

	void SetFriction(float v);
	float GetFriction();

	void AddForce(glm::vec3 force, glm::vec3 pos);
	void AddCentralForce(glm::vec3 force);

	void ApplyImpulse(glm::vec3 impulse);
	void Activate();

	void SetTrigger(bool is_trigg);
	void SetKinematic(bool iskinematic);
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
	virtual void OnSave(YAML::Emitter& out) override;
	virtual void OnLoad(YAML::Node& data) override;

	//bool IsOnGround();
	//bool DoGroundTest(btVector3 contactPos, btVector3 contactNorm);
	// Only if charactar
	//bool _onGround = false;
private:
	
	
};


#endif // R_RIGIDBODY