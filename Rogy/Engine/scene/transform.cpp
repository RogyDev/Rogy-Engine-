#include "transform.h"

// Transformation
glm::quat RTransform::GetQuat()
{
	return glm::quat(Rotation);
}
void  RTransform::SetQuat(float xx, float yy, float zz, float ww)
{
	SetRotation(glm::eulerAngles(glm::quat(xx, yy,zz,ww)));
}
// --------------------------------------------------
void RTransform::SetWorldPosition(glm::vec3 position)
{
	if (parent != nullptr)
		Position = position - parent->GetWorldPosition();
	else
		Position = position;

	m_Dirty = true;
}
// --------------------------------------------------
void RTransform::SetPosition(glm::vec3 position)
{
	if (Position == position) return;
	Position = position;
	m_Dirty = true;
}
// --------------------------------------------------
void RTransform::SetRotation(glm::vec3 rotation)
{
	if (Rotation == rotation) return;
	Rotation = rotation;
	if (Rotation.x >= 360 || Rotation.x <= -360)
		Rotation.x = 0;
	if (Rotation.y >= 360 || Rotation.y <= -360)
		Rotation.y = 0;
	if (Rotation.z >= 360 || Rotation.z <= -360)
		Rotation.z = 0;
	m_Dirty = true;
}
// --------------------------------------------------
void RTransform::SetScale(glm::vec3 scale)
{
	if (Scale == scale) return;
	Scale = scale;
	m_Dirty = true;
}
// --------------------------------------------------
void RTransform::SetScale(float scale)
{
	if (Scale == glm::vec3(scale)) return;
	Scale = glm::vec3(scale);
	m_Dirty = true;
}
// --------------------------------------------------
void RTransform::SetPosition(float xx, float yy, float zz)
{
	SetPosition(glm::vec3(xx,yy,zz));
}
// --------------------------------------------------
void RTransform::SetRotation(float xx, float yy, float zz)
{
	SetRotation(glm::vec3(xx, yy, zz));
}
// --------------------------------------------------
void RTransform::SetScale(float xx, float yy, float zz)
{
	SetScale(glm::vec3(xx, yy, zz));
}
// --------------------------------------------------
glm::vec3 RTransform::GetLocalPosition()
{
	return Position;
}
// --------------------------------------------------
glm::vec3 RTransform::GetLocalRotation()
{
	return Rotation;
}
// --------------------------------------------------
glm::vec3 RTransform::GetLocalScale()
{
	return Scale;
}
// --------------------------------------------------
glm::vec3 RTransform::GetWorldPosition()
{
	if (parent != nullptr)
		return parent->GetWorldPosition() + Position;
	else
		return Position;

	/*glm::mat4 transform = GetTransform();
	glm::vec4 pos = transform * glm::vec4(Position, 1.0f);
	return vec3(pos.x, pos.y, pos.z);*/
}
// --------------------------------------------------
glm::vec3 RTransform::GetWorldScale()
{
	if (parent != nullptr)
		return parent->GetWorldScale() * Scale;
	else
		return Scale;

	/*glm::mat4 transform = GetTransform();
	glm::vec3 scale = glm::vec3(transform[0][0], transform[1][1], transform[2][2]);
	if (scale.x < 0.0f) scale.x *= -1.0f;
	if (scale.y < 0.0f) scale.y *= -1.0f;
	if (scale.z < 0.0f) scale.z *= -1.0f;
	return scale;*/
}
// --------------------------------------------------
glm::mat4 RTransform::GetTransform()
{
	if (m_Dirty)
	{
		UpdateTransform();
	}
	return m_Transform;
}
// --------------------------------------------------
void RTransform::UpdateTransform()
{
	if (m_Dirty)
	{
		// first translation, then rotate, then scale
		m_Transform  = glm::mat4(1);
		m_Transform  = glm::translate(m_Transform, Position);
		m_Transform *= glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
		m_Transform  = glm::scale(m_Transform, Scale);
		
		if (parent)
		{
			m_Transform = parent->m_Transform * m_Transform;
		}
	}
}

// Transform vectors
// --------------------------------------------------
glm::vec3 RTransform::direction()
{
	if (useLocalVecs)
		return DirVec;

	return glm::vec3(cos(Rotation.x) * sin(Rotation.y), sin(Rotation.x), cos(Rotation.x) * cos(Rotation.y));
	//return vec3(2 * (_Rotation.x*_Rotation.z + _Rotation.w*_Rotation.y), 2 * (_Rotation.y*Rotation.z - _Rotation.w*_Rotation.x), 1 - 2 * (_Rotation.x*_Rotation.x + _Rotation.y*_Rotation.y));
}
// --------------------------------------------------
glm::vec3 RTransform::right()
{
	if (useLocalVecs)
		return RightVec;

	return glm::vec3(sin(Rotation.y - 3.14f / 2.0f), Rotation.z, cos(Rotation.y - 3.14f / 2.0f));
	//return -glm::vec3(1 - 2 * (_Rotation.y*_Rotation.y + _Rotation.z*_Rotation.z), 2 * (_Rotation.x*_Rotation.y + _Rotation.w*_Rotation.z), 2 * (_Rotation.x*_Rotation.z - _Rotation.w*_Rotation.y));
}
// --------------------------------------------------
glm::vec3 RTransform::up()
{
	if (useLocalVecs)
		return UpVec;
	// Up vector
	return glm::cross(right(), direction());
}
// --------------------------------------------------
float RTransform::Deg2Rad()
{
	return (3.14f * 2) / 360;
}
// --------------------------------------------------
glm::vec3 RTransform::forward()
{
	return direction();
}
// --------------------------------------------------
