#ifndef R_VEHICLE
#define R_VEHICLE

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include "../scene/transform.h"
#include <string>
#include "../scene/RComponent.h"

class Vehicle : public Component
{
	BASE_COMPONENT()
public:
	Vehicle();
	~Vehicle();
	
	void Init(btRigidBody* body, btDynamicsWorld *pbtDynWorld);
	void AddWheel(glm::vec3 position, glm::vec3 dir, bool isFrontWheel);
	// ------------------------------------------------------------------------

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override;
	virtual void OnLoad(YAML::Node& data) override;

private:
	// Hull RigidBody
	btRigidBody* hullBody_;

	/// Current left/right steering amount (-1 to 1.)
	float steering_;

	// raycast vehicle
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster                  *m_vehicleRayCaster;
	btRaycastVehicle                    *m_vehicle;

	float	m_fEngineForce;
	float	m_fBreakingForce;

	float	m_fmaxEngineForce;
	float	m_fmaxBreakingForce;

	float	m_fVehicleSteering;
	float	m_fsteeringIncrement;
	float	m_fsteeringClamp;
	float	m_fwheelRadius;
	float	m_fwheelWidth;
	float	m_fwheelFriction;
	float	m_fsuspensionStiffness;
	float	m_fsuspensionDamping;
	float	m_fsuspensionCompression;
	float	m_frollInfluence;
	float   m_fsuspensionRestLength;
};


#endif // R_RIGIDBODY