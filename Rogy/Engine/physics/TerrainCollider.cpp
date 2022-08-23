#include "TerrainCollider.h"

IMPL_COMPONENT(TerrainCollider);

//int RigidBody::TYPE_ID = 0;
// ------------------------------------------------------------------------
TerrainCollider::TerrainCollider()
{
}
// ------------------------------------------------------------------------
TerrainCollider::~TerrainCollider()
{
	//std::cout << "RBODY REMOVED DELETED" << std::endl;
}
// ------------------------------------------------------------------------
btRigidBody* TerrainCollider::Init(float* data, int width, float maxHeight, float Size)
{
	if (initialized)
		Clear();

	mData = data;
	/*width = 32;
	maxHeight = 1.0f;
	mData = new float[width * width];
	for (size_t i = 0; i < (width * width); i++)
	{
		mData[i] = 0.0f;
	}

	shape = new btHeightfieldTerrainShape(width, width, mData, 1, 0, 1, 1, PHY_FLOAT, false);
	//shape = new btHeightfieldTerrainShape(width, width, mData, 2.0f, 1, true, false);*/
	shape = new btHeightfieldTerrainShape(width, width, mData, 1, -maxHeight, maxHeight, 1, PHY_FLOAT, false);

	btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
		btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(Size, 0.0f, Size)));
		//btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(50*width/2, 0.0f, 50 * width / 2)));

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		0.0f, // mass, in kg. 0 -> Static object, will never move.
		motionstate,
		shape, // collision shape of body
		btVector3(1, 1, 1) // local inertia
	);
	body = new btRigidBody(rigidBodyCI);
	float width_scale = (1.0f / width) * 2;
	width_scale *= Size;
	shape->setLocalScaling(btVector3(width_scale, 1.0f, width_scale));

	initialized = true;
	return body;
}
// ------------------------------------------------------------------------
void TerrainCollider::Clear()
{
	if (shape == nullptr || initialized == false) return;

	delete shape;
	initialized = false;

}
// ------------------------------------------------------------------------
void TerrainCollider::OnSave(YAML::Emitter& out)
{
	out << YAML::Key << "TerrainCollider" << YAML::BeginMap;

	//out << YAML::Key << "m_CollisionType" << YAML::Value << coll_type;
	//out << YAML::Key << "Gravity"; RYAML::SerVec3(out, Gravity);

	out << YAML::EndMap;
}
// ------------------------------------------------------------------------
void TerrainCollider::OnLoad(YAML::Node& data)
{
	
}
// ------------------------------------------------------------------------