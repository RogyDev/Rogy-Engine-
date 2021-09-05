#include "RTransform.h"
// ------------------------------------------------------------
Transform::Transform()
{
}
// ------------------------------------------------------------
Transform::~Transform()
{
}
// ------------------------------------------------------------
// Transformation
// ------------------------------------------------------------
glm::vec3 Transform::GetEurlerAngelsRad()
{
	return glm::radians(Angels);
}
// ------------------------------------------------------------
glm::vec3 Transform::GetEurlerAngels()
{
	return Angels;
}
// ------------------------------------------------------------
void Transform::SetAngels(glm::vec3 angels)
{
	Angels = angels;
	if (Angels.x >= 360 || Angels.x <= -360)
		Angels.x = 0;
	if (Angels.y >= 360 || Angels.y <= -360)
		Angels.y = 0;
	if (Angels.z >= 360 || Angels.z <= -360)
		Angels.z = 0;

	SetRotation(glm::quat(glm::radians(Angels)));
}
// ------------------------------------------------------------
void Transform::LookAt(glm::vec3 target)
{
	glm::vec3 lookDir = target - GetWorldPosition();
	
	glm::quat newRotation(lookDir, up());
	Rotation = newRotation;
}
// ------------------------------------------------------------
void Transform::Translate(glm::vec3 dir)
{
	SetPosition(GetLocalPosition() + dir);
}
// ------------------------------------------------------------
void Transform::SetLocalScale(glm::vec3 sca)
{
	SetScale(sca);
}
// ------------------------------------------------------------
void Transform::SetMat4(glm::mat4 sMat4)
{

}
// ------------------------------------------------------------
void Transform::SetWorldPosition(glm::vec3 position)
{
	if (parent != nullptr)
		SetPosition(position - parent->GetWorldPosition());
	else
		SetPosition(position);
}
// ------------------------------------------------------------
void Transform::SetPosition(glm::vec3 position)
{
	if (Position == position) return;
	Position = position;
	m_Dirty = true;
}
// ------------------------------------------------------------
void Transform::SetRotation(glm::quat rotation)
{
	if (Rotation == rotation) return;
	Rotation = rotation;
	
	m_Dirty = true;
}
// ------------------------------------------------------------
void Transform::SetScale(glm::vec3 scale)
{
	if (Scale == scale) return;
	Scale = scale;
	m_Dirty = true;
}
// ------------------------------------------------------------
void Transform::SetWorldScale(glm::vec3 sca)
{
	if (parent != nullptr)
		SetScale(sca / parent->GetWorldScale());
	else
		SetScale(sca);
}
// ------------------------------------------------------------
void Transform::SetScaleA(float scale)
{
	if (Scale == glm::vec3(scale)) return;
	Scale = glm::vec3(scale);
	m_Dirty = true;
}
// ------------------------------------------------------------
//void Transform::SetPosition(float xx, float yy, float zz)
//{
	//SetPosition(glm::vec3(xx, yy, zz));
//}
// ------------------------------------------------------------
void Transform::SetRotation(float xx, float yy, float zz)
{
	SetAngels(glm::vec3(xx, yy, zz));
}
// ------------------------------------------------------------
void Transform::SetScale(float xx, float yy, float zz)
{
	SetScale(glm::vec3(xx, yy, zz));
}
// ------------------------------------------------------------
glm::vec3 Transform::GetLocalPosition()
{
	return Position;
}
// ------------------------------------------------------------
glm::quat Transform::GetLocalRotation()
{
	return Rotation;
}
// ------------------------------------------------------------
glm::vec3 Transform::GetLocalScale()
{
	return Scale;
}
// ------------------------------------------------------------
glm::vec3 Transform::GetWorldPosition()
{
	if (parent != nullptr)
		return WPosition;
	else
		return Position;
}
// ------------------------------------------------------------
glm::quat Transform::GetWorldRotation()
{
	if (parent != nullptr)
		return parent->GetWorldRotation() * Rotation;
	else
		return Rotation;
}
// ------------------------------------------------------------
glm::vec3 Transform::GetWorldScale()
{
	if (parent != nullptr)
		return WScale;
		//return Scale;
		//return parent->GetWorldScale() * Scale;
	else
		return Scale;
}
// ------------------------------------------------------------
glm::mat4 Transform::GetTransform()
{
	if (m_Dirty)
	{
		UpdateTransform();
	}
	return m_Transform;
}
// ------------------------------------------------------------
void Transform::UpdateTransform()
{
	m_Last_Transform = m_Transform;

	if (m_Dirty)
	{
		// first translation, then rotate, then scale
		m_Transform = glm::translate(glm::mat4(1.0f), Position);
		m_Orientation = mat4_cast(Rotation);
		m_Transform *= m_Orientation;
		m_Transform = glm::scale(m_Transform, Scale);

		if (parent)
		{
			m_Transform = parent->m_Transform * m_Transform;
			WPosition = glm::vec3(m_Transform[3]);
			WScale = parent->GetWorldScale() * Scale;
		}
		else
		{
			WPosition = Position;
			WScale = Scale;
		}
		transformation_changed = true;

		/*glm::mat4 transformation; // your transformation matrix.
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_Transform, scale, rotation, translation, skew, perspective);
		WPosition = translation;
		WRotation = rotation;
		WScale = scale;*/
	}
}

// Transform vectors
// ------------------------------------------------------------
glm::vec3 Transform::direction()
{
	glm::quat Rot = GetWorldRotation();
	return glm::vec3(2 * (Rot.x * Rot.z + Rot.w * Rot.y), 2 * (Rot.y*Rot.z - Rot.w*Rot.x), 
		1 - 2 * (Rot.x*Rot.x + Rot.y*Rot.y));
}
// ------------------------------------------------------------
glm::vec3 Transform::right()
{
	glm::quat Rot = GetWorldRotation();
	return -glm::vec3(1 - 2 * (Rot.y*Rot.y + Rot.z*Rot.z), 2 * (Rot.x*Rot.y + Rot.w*Rot.z),
		2 * (Rot.x*Rot.z - Rot.w*Rot.y));
}
// ------------------------------------------------------------
glm::vec3 Transform::up()
{
	// Up vector
	return glm::cross(right(), direction());
}
// ------------------------------------------------------------
float Transform::Deg2Rad()
{
	return (3.14f * 2) / 360;
}
// ------------------------------------------------------------
glm::vec3 Transform::forward()
{
	return direction();
}
// ------------------------------------------------------------
void Transform::SerVec3(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
}
// ------------------------------------------------------------
void Transform::SerQuat(YAML::Emitter& out, const glm::quat& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
}
// ------------------------------------------------------------
glm::vec3 Transform::GetVec3(YAML::Node& out)
{
	glm::vec3 v;	
	if (out.IsSequence()) {
		v.x = out[0].as<float>();
		v.y = out[1].as<float>();
		v.z = out[2].as<float>();
	}
	return v;
}
// ------------------------------------------------------------
glm::quat Transform::GetQuat(YAML::Node& out)
{
	glm::quat v;
	if (out.IsSequence()) {
		v.x = out[0].as<float>();
		v.y = out[1].as<float>();
		v.z = out[2].as<float>();
		v.w = out[3].as<float>();
	}
	return v;
}
// ------------------------------------------------------------
void Transform::SerializeSave(YAML::Emitter& out)
{
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Flow;
	out << YAML::Key << "Position"; SerVec3(out, Position);
	out << YAML::Key << "Rotation"; SerQuat(out, Rotation);
	out << YAML::Key << "Scale"; SerVec3(out, Scale);
	out << YAML::Key << "Angels"; SerVec3(out, Angels);
	out << YAML::EndMap;

	//if (!noApply)
}
// ------------------------------------------------------------
void Transform::SerializeLoad(YAML::Node& out)
{
	auto transform = out["Transform"];
	if (transform)
	{
		Position = GetVec3(transform["Position"]);
		Rotation = GetQuat(transform["Rotation"]);
		Scale = GetVec3(transform["Scale"]);
		Angels = GetVec3(transform["Angels"]);
		m_Dirty = true;
	}
}
// ------------------------------------------------------------