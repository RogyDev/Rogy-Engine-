
#ifndef R_PHYSICS_WORLD
#define R_PHYSICS_WORLD

#include "Vehicle.h"
#include "RigidBody.h"
#include "TerrainCollider.h"
#include "../scene/Entity.h"
#include "../mesh/mesh.h"
#include "../scripting/ScriptManager.h"

struct RayHitInfo
{
	bool hasHit;
	float distance;
	glm::vec3 point;
	glm::vec3 normal;
	Entity* body;
};


struct RColMeshShape
{
	std::string mesh_path;
	btTriangleMesh* tm;

	RColMeshShape(btTriangleMesh* t, std::string mp)
	{
		mesh_path = mp;
		tm = t;
	}
};

class PhysicsWorld
{
public:

	struct CollInfo
	{
		unsigned int idA;
		unsigned int idB;
		bool check = false;
		CollInfo(unsigned int aidA,
			unsigned int aidB)
		{
			idA = aidA;
			idB = aidB;
			check = true;
		}
	};

	PhysicsWorld();
	~PhysicsWorld();

	// Build the broadphase
	btBroadphaseInterface* broadphase;

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration;

	btCollisionDispatcher* dispatcher;

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver;

	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<RigidBody*> bodies;

	std::vector<TerrainCollider*> terrains;

	std::vector<RColMeshShape> mesh_cols;

	btTriangleMesh* GetMeshCollider(Mesh* mesh);

	ScriptManager* mScriptManager = nullptr;

	glm::vec3 grv;
	glm::vec3 GetGravity();
	void SetGravity(glm::vec3& val);

	void Init();

	void update();
	void StepSimulation(float dt);
	void updateAABBs();

	bool RayTest(glm::vec3 origin, glm::vec3 dir, float range);
	bool Raycast(glm::vec3 origin, glm::vec3 dir, float range, RayHitInfo* info);
	Entity* RaycastRef(glm::vec3 origin, glm::vec3 dir, float range);
	bool RaycastHitPoint(glm::vec3 origin, glm::vec3 dir, float range, glm::vec3& point);

	bool CheckSphere(glm::vec3 center, glm::vec3 dir, float radius, RayHitInfo* result = nullptr);

	void AddRigidBody(Entity* ent, float Mass = 1.0f);
	RigidBody* GetBody(EnttID entId);
	void RemoveRigidBody(Entity* ent);
	RigidBody* CreateRigidBody(Transform& trans, EnttID entId, float Mass = 1.0f);
	void ChangeShape(RigidBody* rb, RCollisionShapeType toType);

	TerrainCollider* AddTerrainCollider(Entity* ent, float* data, int width, float maxHeight, float terrain_size);

	void ClearRigidbodies();

	static btVector3 ToBtVector3(glm::vec3 v);
	static glm::vec3 ToVec3(btVector3& v);

	void ScreenPosToWorldRay(
		int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
		int screenWidth, int screenHeight,  // Window size, in pixels
		glm::mat4 ViewMatrix,               // Camera position and orientation
		glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
		glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
		glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
	) {

		// The ray Start and End positions, in Normalized Device Coordinates 
		glm::vec4 lRayStart_NDC(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			-((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, 
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
			-((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		/*glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
		glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

		glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;*/

		// Faster way (just one inverse)
		glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
		glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
		glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		out_origin = glm::vec3(lRayStart_world);
		out_direction = glm::normalize(lRayDir_world);
	}

	float TimeStep;
	void PhyTickCallback(btScalar timeStep);
	bool IsPlaying = false;
	
private:
	
	unsigned int id_indx = 0;
	std::vector<PhysicsWorld::CollInfo> current_contacts;
	std::vector<PhysicsWorld::CollInfo> lost_contacts;
};


#endif // R_PHYSICS_WORLD