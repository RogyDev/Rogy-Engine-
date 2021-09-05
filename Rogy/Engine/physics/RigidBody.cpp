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
	vel.y = GetVelocity().y;
	SetVelocity(vel);
	rigidBody->activate();
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
	rigidBody->activate();
	rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() + btVector3(force.x, force.y, force.z));
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
void RigidBody::SetCollisionMesh(btTriangleMesh * tm, std::string& meshPath)
{
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
	tr.setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
	GetMotionState()->setWorldTransform(tr);
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

	//if (is_trigg) rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	//else rigidBody->setCollisionFlags(init_cf);
	rigidBody->setCollisionFlags(flags);
}
// ------------------------------------------------------------------------
void  RigidBody::SetBodyMode(RPhyBodyMode btype)
{
	if (btype != m_BodyMode)
	{
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
	std::cout << rad.getX() << " " << rad.getY() << " " << rad.getZ();
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
	if (mScaleMultiplier == newScale) return;
	if (newScale.x < 0.1f) newScale.x = 0.1f;
	if (newScale.y < 0.1f) newScale.y = 0.1f;
	if (newScale.z < 0.1f) newScale.z = 0.1f;
	mScaleMultiplier = newScale;
	btVector3 rad = btVector3(mScale.x * mScaleMultiplier.x, mScale.y * mScaleMultiplier.y, mScale.z * mScaleMultiplier.z);
	collisionShape->setLocalScaling(rad);
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
	out << YAML::Key << "is_trigger" << YAML::Value << is_trigger;
	out << YAML::Key << "mScaleMultiplier"; RYAML::SerVec3(out, mScaleMultiplier);
	out << YAML::Key << "mOffset"; RYAML::SerVec3(out, mOffset);
	out << YAML::Key << "mesh_path" << YAML::Value << mesh_path;

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

	if(data["mesh_path"].IsDefined())
		mesh_path = data["mesh_path"].as<std::string>();

	SetCollisionShape((RCollisionShapeType)coll_type);
	SetBodyMode((RPhyBodyMode)bdMode);
	SetMass(mss);
	SetDamping(l_damp, ang_damp);
	SetScale(last_scale);
	SetTrigger(isTrigg);
}
// ------------------------------------------------------------------------
