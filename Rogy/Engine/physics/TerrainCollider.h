#ifndef R_TERRAIN_COLLIDER
#define R_TERRAIN_COLLIDER

#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "../scene/transform.h"
#include <string>
#include "../scene/RComponent.h"

class TerrainCollider : public Component
{
	BASE_COMPONENT()
public:
	TerrainCollider();
	~TerrainCollider();
	
	btRigidBody* body = nullptr;
	btHeightfieldTerrainShape* shape = nullptr;
	float* mData = nullptr;

	// ------------------------------------------------------------------------
	btRigidBody* Init(float* data, int width, float maxHeight, float Size);
	void Clear();

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override;
	virtual void OnLoad(YAML::Node& data) override;

private:
	bool initialized = false;
	
};


#endif // R_RIGIDBODY