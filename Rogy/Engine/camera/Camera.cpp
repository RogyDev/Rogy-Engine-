#include "Camera.h"
// --------------------------------------------------------------------------------------------
void Camera::Initia( void )
{
	FarView = 1000.0f;
	NearView = 0.05f;

	// Initial Field of View
	FOV = 70.0f;

	Speed = 5.0f; // 5 units per second
	MouseSpeed = 0.003f;

	MouseSeted = false;
	temp_cam = false;
}
// --------------------------------------------------------------------------------------------
Camera::Camera() {}
// --------------------------------------------------------------------------------------------
Camera::~Camera(){}
// --------------------------------------------------------------------------------------------
Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up)
{
	transform.Position = position;
	transform._forw = forward;
	transform._up = up;
}
// --------------------------------------------------------------------------------------------
void Camera::SetPerspective(float fov, float aspect, float near, float far)
{
	FOV = fov;
	aspect = aspect;
	NearView = near;
	FarView = far;
}
// --------------------------------------------------------------------------------------------
glm::mat4 Camera::GetViewMatrix()
{
	return viewMatrix;
}
// --------------------------------------------------------------------------------------------
glm::mat4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
// --------------------------------------------------------------------------------------------
void Camera::ComputeMatrices()
{
	rot_vector = transform.Rotation - last_rot;

	projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, NearView, FarView);
 
	viewMatrix = glm::lookAt( transform.Position , transform.Position + transform.direction() , transform.up() );

	last_rot = transform.Rotation;
}
// --------------------------------------------------------------------------------------------
void Camera::Save_ini()
{
	if (ini_was_saved) return;

	ini_was_saved = true;
	transform.useLocalVecs = true;
	ini_transform.Position = transform.Position;
	ini_transform.Rotation = transform.Rotation;
	ini_FOV = FOV;
	ini_NearView = NearView;
	ini_FarView = 2000;
}
// --------------------------------------------------------------------------------------------
void Camera::Reset_ini()
{
	if (!ini_was_saved) return;

	ini_was_saved = false;
	transform.useLocalVecs = false;
	transform.Position = ini_transform.Position;
	transform.Rotation = ini_transform.Rotation;
	FOV = ini_FOV;
	NearView = ini_NearView;
	FarView = 2000;
}
// --------------------------------------------------------------------------------------------