#include <glm/glm.hpp>
#include "../scene/transform.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	~Camera();

	void SetPerspective(float fov, float aspect, float near, float far);

	bool MouseSeted;

	RTransform transform;

	glm::vec3 last_rot;
	glm::vec3 rot_vector;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Initial Field of View
	float FOV;

	float Speed; // 3 units / second
	float MouseSpeed;
	float FarView;
	float NearView;
	float aspectRatio;

	RTransform ini_transform;
	float ini_FOV;
	float ini_FarView;
	float ini_NearView;
	bool ini_was_saved;

	void Initia( void );

	void ComputeMatrices();

	glm::mat4 GetViewMatrix();

	glm::mat4 GetProjectionMatrix();

	void Save_ini();
	void Reset_ini();

	bool temp_cam = false;
};

#endif