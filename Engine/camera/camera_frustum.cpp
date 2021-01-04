#include "camera_frustum.h"

// ------------------------------------------------------------------------
void CameraFrustum::Update(Camera* camera)
{
/*	float tan = 2.0 * std::tan(camera->FOV * 0.5);
	float nearHeight = tan * camera->NearView;
	float nearWidth = nearHeight * camera->aspectRatio;
	float farHeight = tan * camera->FarView;
	float farWidth = farHeight * camera->aspectRatio;

	glm::vec3 nearCenter = camera->transform.Position + camera->transform.forward() * camera->NearView;
	glm::vec3 farCenter = camera->transform.Position + camera->transform.forward() * camera->FarView;

	glm::vec3 v;
	// left plane
	v = (nearCenter - camera->transform.right() * nearWidth * 0.5f) - camera->transform.Position;
	Planes[0].SetNormalD(glm::cross(glm::normalize(v), camera->transform.up()), nearCenter - camera->transform.right() * nearWidth * 0.5f);
	// right plane
	v = (nearCenter + camera->transform.right() * nearWidth  * 0.5f) - camera->transform.Position;
	Planes[1].SetNormalD(glm::cross(camera->transform.up(), glm::normalize(v)), nearCenter + camera->transform.right() * nearWidth * 0.5f);
	// top plane
	v = (nearCenter + camera->transform.up() * nearHeight * 0.5f) - camera->transform.Position;
	Planes[2].SetNormalD(glm::cross(glm::normalize(v), camera->transform.right()), nearCenter + camera->transform.up() * nearHeight * 0.5f);
	// bottom plane
	v = (nearCenter - camera->transform.up() * nearHeight * 0.5f) - camera->transform.Position;
	Planes[3].SetNormalD(glm::cross(camera->transform.right(), glm::normalize(v)), nearCenter - camera->transform.up() * nearHeight * 0.5f);
	// near plane
	Planes[4].SetNormalD(camera->transform.forward(), nearCenter);
	// far plane
	Planes[5].SetNormalD(-camera->transform.forward(), farCenter);*/
}

void CameraFrustum::SetFov(const float angle, const float ratio, const float nearD, const float farD)
{
	float m_ang = glm::radians( angle );
	// GOOGLE : BEST WAY TO DO FRUSTUM CALLING (gamedev.net)
}

// ------------------------------------------------------------------------
bool CameraFrustum::Intersect(glm::vec3 point)
{
	for (int i = 0; i < 6; ++i)
	{
		if (Planes[i].Distance(point) < 0)
		{
			return false;
		}
	}
	return true;
}
// ------------------------------------------------------------------------
bool CameraFrustum::Intersect(glm::vec3 point, float radius)
{
	for (int i = 0; i < 6; ++i)
	{
		if (Planes[i].Distance(point) < -radius)
		{
			return false;
		}
	}
	return true;
}
// ------------------------------------------------------------------------
bool CameraFrustum::Intersect(glm::vec3 boxMin, glm::vec3 boxMax)
{
	for (int i = 0; i < 6; ++i)
	{
		glm::vec3 positive = boxMin;
		if (Planes[i].Normal.x >= 0)
		{
			positive.x = boxMax.x;
		}
		if (Planes[i].Normal.y >= 0)
		{
			positive.y = boxMax.y;
		}
		if (Planes[i].Normal.z >= 0)
		{
			positive.z = boxMax.z;
		}
		if (Planes[i].Distance(positive) < 0)
		{
			return false;
		}
	}
	return true;
}
