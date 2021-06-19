#include "PhysicsWorld.h"
// ------------------------------------------------------------------------
PhysicsWorld::PhysicsWorld()
{
	Init();
}
// ------------------------------------------------------------------------
PhysicsWorld::~PhysicsWorld()
{
	ClearRigidbodies();

	// delete dynamics world
	delete dynamicsWorld;

	// delete solver
	delete solver;
	// delete broadphase
	delete broadphase;
	// delete dispatcher
	delete dispatcher;
}
// ------------------------------------------------------------------------
void PhyTickCallback(btDynamicsWorld *world, btScalar timeStep)
{

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0;i < numManifolds;i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);

		btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
		btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();

		btRigidBody *bodyA = btRigidBody::upcast(obA);
		btRigidBody *bodyB = btRigidBody::upcast(obB);

		Entity* eA = (Entity*)bodyA->getUserPointer();
		Entity* eB = (Entity*)bodyB->getUserPointer();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();

				//  NOT implemented - normal on the collided surface.
				//const btVector3& normalOnB = pt.m_normalWorldOnB;

				if (eA != nullptr && eA->m_CollMode != RB_COLLISION_NONE)
				{
					if (eA->m_CollMode == RB_COLLISION_ALWAYS || eA->m_CollMode == RB_COLLISION_ONCE && !eA->IsColliding)
						eA->OnCollision(true, eB, glm::vec3(ptA.getX(), ptA.getY(), ptA.getZ()));
				}

				if (eB != nullptr && eB->m_CollMode != RB_COLLISION_NONE)
				{
					if (eB->m_CollMode == RB_COLLISION_ALWAYS || eB->m_CollMode == RB_COLLISION_ONCE && !eB->IsColliding)
						eB->OnCollision(true, eA, glm::vec3(ptB.getX(), ptB.getY(), ptB.getZ()));
				}

				break;
			}
		}
	}
}
// ------------------------------------------------------------------------
btTriangleMesh* PhysicsWorld::GetMeshCollider(Mesh * mesh)
{
	if (mesh == nullptr)
		return nullptr;
	
	for (size_t i = 0; i < mesh_cols.size(); i++)
	{
		if (mesh_cols[i].mesh_path == mesh->path)
			return mesh_cols[i].tm;
	}

	btTriangleMesh* tm = new btTriangleMesh();
	for (size_t i = 0; i < mesh->indices.size(); i += 3)
	{
		//tm->addIndex(mesh->indices[i]);
		tm->addTriangle(btVector3(mesh->vertices[i].Position.x, mesh->vertices[i].Position.y, mesh->vertices[i].Position.z),
			btVector3(mesh->vertices[i + 1].Position.x, mesh->vertices[i + 1].Position.y, mesh->vertices[i + 1].Position.z),
			btVector3(mesh->vertices[i + 2].Position.x, mesh->vertices[i + 2].Position.y, mesh->vertices[i + 2].Position.z));
	}
	mesh_cols.emplace_back(tm, mesh->path);
	return tm;
}
// ------------------------------------------------------------------------
glm::vec3 PhysicsWorld::GetGravity()
{
	return ToVec3(dynamicsWorld->getGravity());
}
// ------------------------------------------------------------------------
void PhysicsWorld::SetGravity(glm::vec3& val)
{
	dynamicsWorld->setGravity(ToBtVector3(val));
}
// ------------------------------------------------------------------------
void PhysicsWorld::Init()
{
	broadphase				= new btDbvtBroadphase();
	collisionConfiguration  = new btDefaultCollisionConfiguration();
	dispatcher				= new btCollisionDispatcher(collisionConfiguration);
	solver					= new btSequentialImpulseConstraintSolver;
	dynamicsWorld			= new btDiscreteDynamicsWorld(
							  dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -40.81f, 0));
	dynamicsWorld->setInternalTickCallback(PhyTickCallback);
}
// ------------------------------------------------------------------------
void PhysicsWorld::update()
{
	for (size_t i = 0; i < bodies.size(); i++)
	{
		if (bodies[i]->removed)
		{
			dynamicsWorld->removeRigidBody(bodies[i]->rigidBody);
			delete bodies[i];
			bodies.erase(bodies.begin() + i);
		}
	}
}
// ------------------------------------------------------------------------
void PhysicsWorld::StepSimulation(float dt)
{
	dynamicsWorld->stepSimulation(dt);
}
// ------------------------------------------------------------------------
void PhysicsWorld::updateAABBs()
{
	dynamicsWorld->updateAabbs();
}
// ------------------------------------------------------------------------
bool PhysicsWorld::RayTest(glm::vec3 origin, glm::vec3 dir, float range)
{
	glm::vec3 out_end = origin + dir * range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z)
	);
	dynamicsWorld->rayTest(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z),
		RayCallback
	);
	return RayCallback.hasHit();
}
// ------------------------------------------------------------------------
bool PhysicsWorld::Raycast(glm::vec3 origin, glm::vec3 dir, float range, RayHitInfo* info)
{
	glm::vec3 out_end = origin + dir * range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z)
	);
	dynamicsWorld->rayTest(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z),
		RayCallback
	);

	bool hasHit = RayCallback.hasHit();
	info->hasHit = hasHit;
	if (hasHit)
	{
		info->point.x = RayCallback.m_hitPointWorld.getX();
		info->point.y = RayCallback.m_hitPointWorld.getY();
		info->point.z = RayCallback.m_hitPointWorld.getZ();

		info->normal.x = RayCallback.m_hitNormalWorld.getX();
		info->normal.y = RayCallback.m_hitNormalWorld.getY();
		info->normal.z = RayCallback.m_hitNormalWorld.getZ();
		
		info->distance = glm::distance(origin, info->point);
		info->body = (Entity*)RayCallback.m_collisionObject->getUserPointer();
	}
	return hasHit;
}
// ------------------------------------------------------------------------
Entity* PhysicsWorld::RaycastRef(glm::vec3 origin, glm::vec3 dir, float range)
{
	glm::vec3 out_end = origin + dir * range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z)
	);
	dynamicsWorld->rayTest(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z),
		RayCallback
	);

	bool hasHit = RayCallback.hasHit();
	if (hasHit)
	{
		return (Entity*)RayCallback.m_collisionObject->getUserPointer();
	}
	return nullptr;
}
// ------------------------------------------------------------------------
bool PhysicsWorld::RaycastHitPoint(glm::vec3 origin, glm::vec3 dir, float range, glm::vec3& point)
{
	glm::vec3 out_end = origin + dir * range;
	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z)
	);
	dynamicsWorld->rayTest(
		btVector3(origin.x, origin.y, origin.z),
		btVector3(out_end.x, out_end.y, out_end.z),
		RayCallback
	);

	bool hasHit = RayCallback.hasHit();
	if (hasHit)
	{
		point = glm::vec3(RayCallback.m_hitPointWorld.getX(),
		RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ());
		 return true;
	}
	return false;
}
// ------------------------------------------------------------------------
RigidBody* PhysicsWorld::GetBody(EnttID entId)
{
	for (size_t i = 0; i < bodies.size(); i++)
	{
		if (bodies[i]->entid == entId)
			return bodies[i];
	}
	return nullptr;
}
bool PhysicsWorld::CheckSphere(glm::vec3 center, float radius)
{
	/*btSphereShape sphereShape();
	btRigidBody* tempRigidBody = new btRigidBody(1.0f, nullptr, &sphereShape);
	tempRigidBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), ToBtVector3(center)));
	// Need to activate the temporary rigid body to get reliable results from static, sleeping objects
	tempRigidBody->activate();
	dynamicsWorld->addRigidBody(tempRigidBody);

	btCollisionWorld::ContactResultCallback callback;
	dynamicsWorld->contactTest(tempRigidBody, callback);

	dynamicsWorld->removeRigidBody(tempRigidBody);
	delete tempRigidBody;
	*/
	return false;
}
// ------------------------------------------------------------------------
void  PhysicsWorld::AddRigidBody(Entity* ent, float Mass)
{
	if (ent->HasComponent<RigidBody>())
		return;
	
	ent->AddComponent<RigidBody>(CreateRigidBody(ent->transform, ent->ID, Mass));

	RigidBody* rb = GetBody(ent->ID);

	if (rb)
	{
		rb->rigidBody->setUserPointer(ent);
		rb->collisionShape->setUserPointer(ent);
	}
}
// ------------------------------------------------------------------------
void PhysicsWorld::RemoveRigidBody(Entity* ent)
{
	if (!ent->HasComponent<RigidBody>())
		return;
	
	for (size_t i = 0; i < bodies.size(); i++)
	{
		if (bodies[i]->entid == ent->ID)
		{
			dynamicsWorld->removeRigidBody(bodies[i]->rigidBody);
			delete bodies[i];
			bodies.erase(bodies.begin() + i);
		}
	}

	ent->RemoveComponent<RigidBody>();
}
// ------------------------------------------------------------------------
RigidBody* PhysicsWorld::CreateRigidBody(Transform& trans, EnttID entId, float Mass)
{
	glm::quat qt = glm::quat(trans.GetEurlerAngelsRad());
	//glm::quat qt = trans.GetLocalRotation();
	glm::vec3 pos   = trans.GetLocalPosition();
	glm::vec3 scale = trans.GetLocalScale();

	RigidBody* rb = new RigidBody();
	rb->setedAngels = trans.GetEurlerAngels();
	rb->entid = entId;

	rb->Mass = Mass;
	rb->id = id_indx; id_indx++;
	
	rb->m_CollisionType = BOX_COLLIDER;

	if(Mass == 0)
		rb->m_BodyMode = RB_STATIC;
	else
		rb->m_BodyMode = RB_RIGID;

	//rb->collisionShape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
	rb->collisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	
	btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
		btQuaternion(qt.x, qt.y, qt.z, qt.w), btVector3(pos.x, pos.y, pos.z)));
	
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		Mass, // mass, in kg. 0 -> Static object, will never move.
		motionstate,
		rb->collisionShape, // collision shape of body
		btVector3(1, 1, 1) // local inertia
	);
	rb->rigidBody = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(rb->rigidBody);

	rb->rigidBody->setFriction(1.0f);
	rb->init_cf = rb->rigidBody->getCollisionFlags();

	rb->SetDamping(rb->LinerDamping, rb->AngulerDamping);

	bodies.push_back(rb);
	
	return rb;
}
// ------------------------------------------------------------------------
void PhysicsWorld::ChangeShape(RigidBody* rb, RCollisionShapeType toType)
{
	if (rb->m_CollisionType == toType)
		return;

	delete rb->collisionShape;
	if (toType == BOX_COLLIDER || toType == MESH_COLLIDER)
	{
		rb->collisionShape = new btBoxShape(btVector3(1, 1, 1));
		
	}
	if (toType == SPHERE_COLLIDER)
	{
		rb->collisionShape = new btSphereShape(1.0f);
	}
	if (toType == CAPSULE_COLLIDER)
	{
		rb->collisionShape = new btCapsuleShape(1.0f, 2.0f);
	}
	rb->GetRigidBody()->setCollisionShape(rb->collisionShape);
	
	rb->m_CollisionType = toType;
}
// ------------------------------------------------------------------------
void PhysicsWorld::ClearRigidbodies()
{
	// remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject * obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody * body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	bodies.clear();
}
// ------------------------------------------------------------------------
btVector3 PhysicsWorld::ToBtVector3(glm::vec3 v)
{
	return btVector3(v.x, v.y, v.z);
}
// ------------------------------------------------------------------------
glm::vec3 PhysicsWorld::ToVec3(btVector3& v)
{
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}
// ------------------------------------------------------------------------