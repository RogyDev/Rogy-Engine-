#include "RigidBody.h"

IMPL_COMPONENT(RigidBody);

//int RigidBody::TYPE_ID = 0;
// ------------------------------------------------------------------------
RigidBody::RigidBody()
{
}
// ------------------------------------------------------------------------
RigidBody::~RigidBody()
{
	//std::cout << "RBODY REMOVED DELETED" << std::endl;
}
// ------------------------------------------------------------------------
glm::vec3 RigidBody::GetVelocity()
{
	btVector3 vel = rigidBody->getLinearVelocity();
	return glm::vec3(vel.getX(), vel.getY(), vel.getZ());
}
// ------------------------------------------------------------------------
void RigidBody::SetVelocity(glm::vec3 vel)
{
	rigidBody->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
}
// ------------------------------------------------------------------------
void RigidBody::Move(glm::vec3 vel)
{
	if (Gravity.y != 0.0f)
		vel.y = GetVelocity().y;
	SetVelocity(vel);
	Activate();
}
// ------------------------------------------------------------------------
void RigidBody::SetGravityY(float g)
{
	Gravity.y = g;
	SetGravity(Gravity);
}
// ------------------------------------------------------------------------
void RigidBody::SetGravity(glm::vec3 g)
{
	Gravity = g;
	rigidBody->setGravity(btVector3(g.x, g.y, g.z));
}
// ------------------------------------------------------------------------
glm::vec3 RigidBody::GetGravity()
{
	return Gravity;
}
// ------------------------------------------------------------------------
void RigidBody::SetBounciness(float v)
{
	if (Bounciness == v) return;
	Bounciness = v;
	rigidBody->setRestitution(Bounciness);
}
float RigidBody::GetBounciness()
{
	return Bounciness;
}
// ------------------------------------------------------------------------
void RigidBody::SetFriction(float v)
{
	if (Friction == v) return;
	Friction = v;
	rigidBody->setFriction(Friction);
}
float RigidBody::GetFriction()
{
	return Friction;
}
// ------------------------------------------------------------------------
void RigidBody::AddForce(glm::vec3 force, glm::vec3 pos)
{
	rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(pos.x, pos.y, pos.z));
}
// ------------------------------------------------------------------------
void RigidBody::AddCentralForce(glm::vec3 force)
{
	//rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
	Activate();
	rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() + btVector3(force.x, force.y, force.z));
}
// ------------------------------------------------------------------------
void RigidBody::ApplyImpulse(glm::vec3 impulse)
{
	Activate();
	rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
}
// ------------------------------------------------------------------------
void RigidBody::Activate()
{
	if (Mass > 0.0f)
		rigidBody->activate();
}
// ------------------------------------------------------------------------
btCollisionShape* RigidBody::GetCollisionShape()
{
	return this->collisionShape;
}
// ------------------------------------------------------------------------
btRigidBody* RigidBody::GetRigidBody()
{
	return this->rigidBody;
}
// ------------------------------------------------------------------------
btMotionState* RigidBody::GetMotionState()
{
	return rigidBody->getMotionState();
}
// ------------------------------------------------------------------------
void RigidBody::SetCollisionShape(RCollisionShapeType col_type)
{
	if (collisionShape != nullptr)
	{
		if (m_CollisionType == col_type)
			return;

		void* userPointer = collisionShape->getUserPointer();

		delete collisionShape;
		m_CollisionType = col_type;
		if (m_CollisionType == BOX_COLLIDER)
		{
			collisionShape = new btBoxShape(btVector3(1, 1, 1));
		}
		else if (m_CollisionType == SPHERE_COLLIDER)
		{
			collisionShape = new btSphereShape(1);
		}
		else if (m_CollisionType == CAPSULE_COLLIDER)
		{
			collisionShape = new btCapsuleShape(1, 2);
		}
		else if (m_CollisionType == MESH_COLLIDER)
		{
			collisionShape = nullptr;
		}

		if (collisionShape != nullptr) {
			rigidBody->setCollisionShape(collisionShape);
			collisionShape->setUserPointer(userPointer);
		}
	}
}
// ------------------------------------------------------------------------
void RigidBody::SetCollisionMesh(btTriangleMesh * tm, std::string& meshPath, int indx)
{
	mesh_index = indx;
	mesh_path = meshPath;
	SetCollisionShape(RCollisionShapeType::MESH_COLLIDER);

	btCollisionShape* meshShape = new btBvhTriangleMeshShape(tm, true);
	collisionShape = meshShape;
	rigidBody->setCollisionShape(collisionShape);
}
// ------------------------------------------------------------------------
btTransform& RigidBody::GetTransform()
{
	return this->rigidBody->getWorldTransform();
}
// ------------------------------------------------------------------------
void RigidBody::SetMass(float ms)
{
	if (m_BodyMode == RB_STATIC)
		return;

	if (Mass != ms)
	{
		Mass = ms;
		rigidBody->setMassProps(Mass, btVector3(inertia.x, inertia.y, inertia.z));
	}
}
// ------------------------------------------------------------------------
void RigidBody::SetDamping(float l_damp, float ang_damp)
{
	if (l_damp != LinerDamping || ang_damp != AngulerDamping)
	{
		LinerDamping = l_damp;
		AngulerDamping = ang_damp;
		rigidBody->setDamping(LinerDamping, AngulerDamping);
	}
}
// ------------------------------------------------------------------------
void RigidBody::SetPosition(glm::vec3 & newPos)
{
	btTransform& tr = GetTransform();
	tr.setOrigin(btVector3(newPos.x + mOffset.x, newPos.y + mOffset.y, newPos.z + mOffset.z));
	GetMotionState()->setWorldTransform(tr);
}
// ------------------------------------------------------------------------
glm::vec3 RigidBody::GetPosition()
{
	return glm::vec3();
}
// ------------------------------------------------------------------------
void RigidBody::SetAngels(glm::vec3& angles)
{
	btTransform& tr = GetTransform();

	if (angles.x >= 360 || angles.x <= -360)
		angles.x = 0;
	if (angles.y >= 360 || angles.y <= -360)
		angles.y = 0;
	if (angles.z >= 360 || angles.z <= -360)
		angles.z = 0;

	glm::quat pos = glm::quat(glm::radians(angles));

	tr.setRotation(btQuaternion(pos.x, pos.y, pos.z, pos.w));
	GetMotionState()->setWorldTransform(tr);
	setedAngels = angles;
}
// ------------------------------------------------------------------------
void RigidBody::SetRotation(glm::quat& rotation)
{
	btTransform& tr = GetTransform();
	tr.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	GetMotionState()->setWorldTransform(tr);
}
// ------------------------------------------------------------------------
void RigidBody::SetTrigger(bool is_trigg)
{
	if (is_trigg == is_trigger) return;
	is_trigger = is_trigg;
	int flags = rigidBody->getCollisionFlags();

	if (is_trigg) flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
	else flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

	if (is_kinematic)
		flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	else
		flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
	//if (is_trigg) rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	//else rigidBody->setCollisionFlags(init_cf);
	rigidBody->setCollisionFlags(flags);
}
void RigidBody::SetKinematic(bool iskinematic)
{
	if (iskinematic == is_kinematic) return;
	is_kinematic = iskinematic;
	int flags = rigidBody->getCollisionFlags();

	if (is_kinematic)
		flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	else
		flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

	rigidBody->setCollisionFlags(flags);
	rigidBody->forceActivationState(is_kinematic ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);
}
// ------------------------------------------------------------------------
void  RigidBody::SetBodyMode(RPhyBodyMode btype)
{
	if (btype != m_BodyMode)
	{
		rigidBody->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
		if (btype == RB_RIGID)
		{
			inertia = glm::vec3(1.0f, 1.0f, 1.0f);
			rigidBody->setMassProps(Mass, btVector3(inertia.x, inertia.y, inertia.z));
		}
		else if (btype == RB_STATIC)
		{
			inertia = glm::vec3(1.0f, 1.0f, 1.0f);
			rigidBody->setMassProps(0.0f, btVector3(inertia.x, inertia.y, inertia.z));
		}
		else if (btype == RB_CHARACTAR)
		{
			inertia = glm::vec3(0.0f, 0.0f, 0.0f);
			rigidBody->setMassProps(Mass, btVector3(inertia.x, inertia.y, inertia.z));
			rigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
		}
		m_BodyMode = btype;
	}
}
// ------------------------------------------------------------------------
void RigidBody::ChangeShape(RCollisionShapeType toType)
{
	if (m_CollisionType == toType)
		return;

	delete collisionShape;
	if (toType == BOX_COLLIDER || toType == MESH_COLLIDER)
	{
		collisionShape = new btBoxShape(btVector3(1, 1, 1));

	}
	if (toType == SPHERE_COLLIDER)
	{
		collisionShape = new btSphereShape(1.0f);
	}
	if (toType == CAPSULE_COLLIDER)
	{
		collisionShape = new btCapsuleShape(1.0f, 2.0f);
	}
	GetRigidBody()->setCollisionShape(collisionShape);

	m_CollisionType = toType;
}
// ------------------------------------------------------------------------
void RigidBody::SetCollisionScale(glm::vec3 newScale)
{
	if (mScale == newScale) return;
	if (newScale.x < 0.001f) newScale.x = 0.001f;
	if (newScale.y < 0.001f) newScale.y = 0.001f;
	if (newScale.z < 0.001f) newScale.z = 0.001f;
	mScale = newScale;
	btVector3 rad = btVector3(mScale.x * mScaleMultiplier.x, mScale.y * mScaleMultiplier.y, mScale.z * mScaleMultiplier.z);
	//std::cout << rad.getX() << " " << rad.getY() << " " << rad.getZ();
	if(collisionShape != nullptr)
		collisionShape->setLocalScaling(rad);
}
// ------------------------------------------------------------------------
glm::vec3& RigidBody::GetScale()
{
	return mScaleMultiplier;
}
// ------------------------------------------------------------------------
void RigidBody::SetScale(glm::vec3 newScale)
{
	//if (mScaleMultiplier == newScale) return;
	if (newScale.x < 0.001f) newScale.x = 0.001f;
	if (newScale.y < 0.001f) newScale.y = 0.001f;
	if (newScale.z < 0.001f) newScale.z = 0.001f;
	mScaleMultiplier = newScale;
	btVector3 rad = btVector3(mScale.x * mScaleMultiplier.x, mScale.y * mScaleMultiplier.y, mScale.z * mScaleMultiplier.z);
	if (collisionShape != nullptr) collisionShape->setLocalScaling(rad);
	Activate();
}
// ------------------------------------------------------------------------
void RigidBody::OnSave(YAML::Emitter& out)
{
	out << YAML::Key << "RigidBody" << YAML::BeginMap;

	int coll_type = int(m_CollisionType);
	int bdMode = int(m_BodyMode);
	out << YAML::Key << "m_CollisionType" << YAML::Value << coll_type;
	out << YAML::Key << "m_BodyMode" << YAML::Value << bdMode;
	out << YAML::Key << "Mass" << YAML::Value << Mass;
	out << YAML::Key << "LinerDamping" << YAML::Value << LinerDamping;
	out << YAML::Key << "AngulerDamping" << YAML::Value << AngulerDamping;
	out << YAML::Key << "Bounciness" << YAML::Value << Bounciness;
	out << YAML::Key << "Friction" << YAML::Value << Friction;
	out << YAML::Key << "is_trigger" << YAML::Value << is_trigger;
	out << YAML::Key << "is_kinematic" << YAML::Value << is_kinematic;
	out << YAML::Key << "mScaleMultiplier"; RYAML::SerVec3(out, mScaleMultiplier);
	out << YAML::Key << "mOffset"; RYAML::SerVec3(out, mOffset);
	out << YAML::Key << "mesh_path" << YAML::Value << mesh_path;
	out << YAML::Key << "mesh_index" << YAML::Value << mesh_index;
	//out << YAML::Key << "Gravity"; RYAML::SerVec3(out, Gravity);

	out << YAML::EndMap;
}
// ------------------------------------------------------------------------
void RigidBody::OnLoad(YAML::Node& data)
{
	int coll_type, bdMode;
	float mss, l_damp, ang_damp;
	bool isTrigg;
	glm::vec3 last_scale;

	coll_type = data["m_CollisionType"].as<int>();
	bdMode = data["m_BodyMode"].as<int>();
	mss = data["Mass"].as<float>();
	l_damp = data["LinerDamping"].as<float>();
	ang_damp = data["AngulerDamping"].as<float>();
	isTrigg = data["is_trigger"].as<bool>();
	last_scale = RYAML::GetVec3(data["mScaleMultiplier"]);
	mOffset = RYAML::GetVec3(data["mOffset"]);

	if (data["is_kinematic"].IsDefined())
		is_kinematic = data["is_kinematic"].as<bool>();

	if(data["mesh_path"].IsDefined())
		mesh_path = data["mesh_path"].as<std::string>();

	if (data["mesh_index"].IsDefined())
		mesh_index = data["mesh_index"].as<int>();

	if (data["Bounciness"].IsDefined())
		SetBounciness(data["Bounciness"].as<float>());

	if (data["Friction"].IsDefined())
		SetFriction(data["Friction"].as<float>());

	//if (data["Gravity"].IsDefined())
	//	Gravity = RYAML::GetVec3(data["Gravity"]);

	//SetGravity(Gravity);
	SetCollisionShape((RCollisionShapeType)coll_type);
	SetBodyMode((RPhyBodyMode)bdMode);
	SetMass(mss);
	SetDamping(l_damp, ang_damp);
	SetScale(last_scale);
	SetTrigger(isTrigg);
	SetKinematic(is_kinematic);
}
// ------------------------------------------------------------------------
/*bool RigidBody::IsOnGround()
{
	return _onGround;
}
// ------------------------------------------------------------------------
bool RigidBody::DoGroundTest(btVector3 contactPos, btVector3 contactNorm)
{
	_onGround = false;
	glm::vec3& pos = GetPosition();
	// If contact is below node center and pointing up, assume it's a ground contact
	if (contactPos.getY() < (pos.y + 1.0f))
	{
		float level = contactNorm.getY();
		if (level > 0.75)
			_onGround = true;
	}
	return _onGround;
}
// ------------------------------------------------------------------------
*/