#include "Vehicle.h"

IMPL_COMPONENT(Vehicle);

// ------------------------------------------------------------------------
Vehicle::Vehicle()
{
}
// ------------------------------------------------------------------------
Vehicle::~Vehicle()
{
	//std::cout << "RBODY REMOVED DELETED" << std::endl;
}
// ------------------------------------------------------------------------
void Vehicle::Init(btRigidBody * body, btDynamicsWorld *pbtDynWorld)
{
	m_vehicleRayCaster = new btDefaultVehicleRaycaster(pbtDynWorld);
	m_vehicle = new btRaycastVehicle(m_tuning, hullBody_, m_vehicleRayCaster);
	pbtDynWorld->addVehicle(m_vehicle);
}
// ------------------------------------------------------------------------
void Vehicle::AddWheel(glm::vec3 position, glm::vec3 dir, bool isFrontWheel)
{
	//btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3f*m_fwheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - m_fwheelRadius);
	//m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, m_fsuspensionRestLength, m_fwheelRadius, m_tuning, isFrontWheel);
}
// ------------------------------------------------------------------------
void Vehicle::OnSave(YAML::Emitter& out)
{
	out << YAML::Key << "Vehicle" << YAML::BeginMap;
	out << YAML::EndMap;
}
// ------------------------------------------------------------------------
void Vehicle::OnLoad(YAML::Node& data)
{

}
// ------------------------------------------------------------------------
