#include "ScriptAPI.h"


#include "LMath.h"
#include "../input/InputManager.h"
#include "../scene/Scene.h"
#include "../audio/AudioManager.h"
#include "../renderer/lighting.h"
#include "../renderer/cameraComponent.h"
#include "../io/PlayerPrefs.h"
#include "../physics/PhysicsWorld.h"
#include "../ui/UIWidget.h"
#include "../core/Random.h"
#include "../core/Vec3.h"
#include "../renderer/billboardComponent.h"
#include "../renderer/ParticleSystem.h"
#include "../renderer/reflection_probe.h"

namespace EngineAPI_LUA
{
	void RegisterInputAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<InputManager>("InputManager")
			.addFunction("GetKey", &InputManager::GetKey)
			.addFunction("GetKeyDown", &InputManager::GetKeyDown)
			.addFunction("GetKeyUp", &InputManager::GetKeyUp)
			.addFunction("GetAxis", &InputManager::GetAxis)
			.addFunction("GetMouseXPos", &InputManager::GetMouseXPos)
			.addFunction("GetMouseYPos", &InputManager::GetMouseYPos)
			.addFunction("GetMouseXDelta", &InputManager::GetMouseXDelta)
			.addFunction("GetMouseYDelta", &InputManager::GetMouseYDelta)
			.addFunction("GetMouseButton", &InputManager::GetMouseButton)
			.addFunction("GetMouseButtonDown", &InputManager::GetMouseButtonDown)
			.addFunction("GetMouseButtonUp", &InputManager::GetMouseButtonUp)
			.addFunction("SetCursor", &InputManager::SetCursor)
			.endClass();
	}
	void RegisterDebugAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<DebugTool>("DebugTool")
			.addFunction("Log", &DebugTool::Log)
			.addFunction("Warning", &DebugTool::Warning)
			.addFunction("Error", &DebugTool::Error)
			.addFunction("Clear", &DebugTool::Clear)
			.endClass();
	}

	void RegisterUIAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)

			.beginClass<UIWidgetImage>("UIWidgetImage")
			.addProperty("Enabled", &UIWidgetImage::Enabled)
			.addProperty("name", &UIWidgetImage::name)
			.addProperty("Alpha", &UIWidgetImage::Alpha)
			.endClass()

			.beginClass<UIWidgetText>("UIWidgetText")
			.addProperty("Enabled", &UIWidgetText::Enabled)
			.addProperty("name", &UIWidgetText::name)
			.addProperty("Position", &UIWidgetText::Position)
			.addProperty("text", &UIWidgetText::text)
			.endClass()

			.beginClass<UIWidgetButton>("UIWidgetButton")
			.addProperty("Enabled", &UIWidgetButton::Enabled)
			.addProperty("name", &UIWidgetButton::name)
			.endClass()

			.beginClass<UIWidgetProgressBar>("UIWidgetProgressBar")
			.addProperty("Enabled", &UIWidgetProgressBar::Enabled)
			.addProperty("name", &UIWidgetProgressBar::name)
			.addProperty("value", &UIWidgetProgressBar::Value)
			.endClass()

			.beginClass<UIWidget>("UIWidget")
			.addProperty("Alpha", &UIWidget::Alpha)
			.addProperty("Position", &UIWidget::Position)
			.addProperty("Enabled", &UIWidget::enabled)
			.addFunction("GetText", &UIWidget::GetWidgetWithName<UIWidgetText>)
			.addFunction("GetButton", &UIWidget::GetWidgetWithName<UIWidgetButton>)
			.addFunction("GetBar", &UIWidget::GetWidgetWithName<UIWidgetProgressBar>)
			.addFunction("GetImage", &UIWidget::GetWidgetWithName<UIWidgetImage>)
			.endClass()
			;
	}

	void RegisterRendererAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<PointLight>("PointLight")
			.addProperty("Active", &PointLight::Active)
			.addProperty("Intensity", &PointLight::Intensity)
			.addProperty("Color", &PointLight::Color)
			.addProperty("Raduis", &PointLight::Raduis)
			.addProperty("CastShadows", &PointLight::CastShadows)
			.addProperty("Bias", &PointLight::Bias)
			.endClass()

			.beginClass<SpotLight>("SpotLight")
			.addProperty("Active", &SpotLight::Active)
			.addProperty("Intensity", &SpotLight::Intensity)
			.addProperty("Color", &SpotLight::Color)
			.addProperty("Raduis", &SpotLight::Raduis)
			.addProperty("CastShadows", &SpotLight::CastShadows)
			.addProperty("Bias", &SpotLight::Bias)
			.addProperty("OuterCutOff", &SpotLight::OuterCutOff)
			.addProperty("CutOff", &SpotLight::CutOff)
			.endClass()

			.beginClass<DirectionalLight>("DirectionalLight")
			.addProperty("Intensity", &DirectionalLight::Intensity)
			.addProperty("Color", &DirectionalLight::Color)
			.addProperty("CastShadows", &DirectionalLight::CastShadows)
			.addProperty("Soft", &DirectionalLight::Soft)
			.addProperty("Bias", &DirectionalLight::Bias)
			.endClass()

			.beginClass<CameraComponent>("CameraComponent")
			.addProperty("FOV", &CameraComponent::FOV)
			.addProperty("Primary", &CameraComponent::Primary)
			.addProperty("FarView", &CameraComponent::FarView)
			.addProperty("NearView", &CameraComponent::NearView)
			.endClass()

			.beginClass<BillboardComponent>("BillboardComponent")
			.addProperty("enabled", &BillboardComponent::enabled)
			.addProperty("depth_test", &BillboardComponent::depth_test)
			.addProperty("color", &BillboardComponent::color)
			//.addProperty("size", &BillboardComponent::size)
			.addProperty("tex_path", &BillboardComponent::tex_path)
			.addProperty("sun_source", &BillboardComponent::sun_source)
			.endClass()

			.beginClass<ReflectionProbe>("ReflectionProbe")
			.addProperty("BoxProjection", &ReflectionProbe::BoxProjection)
			.addProperty("Intensity", &ReflectionProbe::Intensity)
			.addProperty("static_only", &ReflectionProbe::static_only)
			.addProperty("Resolution", &ReflectionProbe::Resolution)
			.addFunction("BakeReflections", &ReflectionProbe::BakeReflections)
			.endClass()

			.beginClass<ParticleSystem>("ParticleSystem")
			.addProperty("StartSize", &ParticleSystem::StartSize)
			.addProperty("StartColor", &ParticleSystem::StartColor)
			.addProperty("StartSpeed", &ParticleSystem::StartSpeed)
			.addProperty("StartLifeTime", &ParticleSystem::StartLifeTime)
			.addProperty("FadeOut", &ParticleSystem::FadeOut)
			.addProperty("PlayOnStart", &ParticleSystem::PlayOnStart)
			.addProperty("AddSizeOverTime", &ParticleSystem::AddSizeOverTime)
			.addProperty("EmitteCount", &ParticleSystem::EmitteCount)
			.addFunction("GetMaxParticleCount", &ParticleSystem::GetMaxParticleCount)
			.addFunction("SetMaxParticleCount", &ParticleSystem::SetMaxParticleCount)
			.addFunction("Emitte", &ParticleSystem::Emitte)
			.addFunction("Stop", &ParticleSystem::Stop)
			.addProperty("GravityModifier", &ParticleSystem::GravityModifier)
			.addProperty("OneDirection", &ParticleSystem::OneDirection)
			.addProperty("Spread", &ParticleSystem::Spread)
			.addProperty("Looping", &ParticleSystem::Looping)
			.addProperty("Emitting", &ParticleSystem::Emitting)
			.endClass()
			;
	}

	void RegisterMathAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			// Vector3
			.beginClass<glm::vec3>("Vector3")
			.addConstructor<void(*) (float, float, float)>()
			.addProperty("x", &Vec3Helper::get<0>, &Vec3Helper::set<0>)
			.addProperty("y", &Vec3Helper::get<1>, &Vec3Helper::set<1>)
			.addProperty("z", &Vec3Helper::get<2>, &Vec3Helper::set<2>)
			.addFunction("__add", &Vec3Helper::ADD)
			.addFunction("__mul", &Vec3Helper::MUL)
			.addFunction("__div", &Vec3Helper::DIV)
			.addFunction("__sub", &Vec3Helper::SUB)
			.addFunction("__lt", &Vec3Helper::lessThen)
			.addFunction("__le", &Vec3Helper::LessOrEq)
			.addFunction("__eq", &Vec3Helper::Equale)
			.addFunction("GetType", &Vec3Helper::__gettype)
			.addStaticFunction("Down", &Vec3Helper::VecDown)
			.addStaticFunction("Left", &Vec3Helper::VecLeft)
			.addStaticFunction("Up", &Vec3Helper::VecUp)
			.addStaticFunction("Right", &Vec3Helper::VecRight)
			.addStaticFunction("Forward", &Vec3Helper::VecForward)
			.addStaticFunction("Vec3F", &Vec3Helper::Vec3F)
			.addStaticFunction("Lerp", &Vec3Helper::Lerp)
			.addStaticFunction("Distance", &Vec3Helper::Distance)
			.addStaticFunction("Normalize", &Vec3Helper::Normalize)
			.addStaticFunction("CrossProduct", &Vec3Helper::CrossProduct)
			.addStaticFunction("ProjectOnPlane", &Vec3Helper::ProjectOnPlane)
			.endClass()

			// Vector2
			.beginClass<glm::vec2>("Vector2")
			.addConstructor<void(*) (float, float)>()
			.addProperty("x", &Vec2Helper::get<0>, &Vec2Helper::set<0>)
			.addProperty("y", &Vec2Helper::get<1>, &Vec2Helper::set<1>)
			.addFunction("__add", &Vec2Helper::ADD)
			.addFunction("__mul", &Vec2Helper::MUL)
			.addFunction("__div", &Vec2Helper::DIV)
			.addFunction("__sub", &Vec2Helper::SUB)
			.addFunction("__lt", &Vec2Helper::lessThen)
			.addFunction("__le", &Vec2Helper::LessOrEq)
			.addFunction("__eq", &Vec2Helper::Equale)
			.addStaticFunction("Up", &Vec2Helper::VecUp)
			.addStaticFunction("Right", &Vec2Helper::VecRight)
			.addStaticFunction("Vec2F", &Vec2Helper::Vec2F)
			.addStaticFunction("Lerp", &Vec2Helper::Lerp)
			.endClass();

		luabridge::getGlobalNamespace(L).
			beginNamespace("Mathf")
			.addFunction("Clamp", &LMath::Clamp)
			.addFunction("Abs", &LMath::Abs)
			.addFunction("Sin", &LMath::Sin)
			.addFunction("PI", &LMath::PI)
			.addFunction("Random", &Random::Range)
			.addFunction("Sqrt", &LMath::Sqrt)
			.addFunction("LerpV", &LMath::LerpV)
			.addFunction("Lerp", &LMath::Lerp)
			.endNamespace();
	}

	void RegisterAudioAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)

			.beginClass<RAudioSource>("RAudioSource")
			.addFunction("Play", &RAudioSource::Play)
			.addFunction("Stop", &RAudioSource::Stop)
			.addProperty("PlayOnStart", &RAudioSource::PlayOnStart)
			.addFunction("SetLooping", &RAudioSource::SetLooping)
			.addFunction("GetLooping", &RAudioSource::GetLooping)
			.addFunction("SetMaxDistance", &RAudioSource::SetMaxDistance)
			.addFunction("GetMaxDistance", &RAudioSource::GetMaxDistance)
			.addFunction("SetMinDistance", &RAudioSource::SetMinDistance)
			.addFunction("GetMinDistance", &RAudioSource::GetMinDistance)
			.addFunction("SetVolume", &RAudioSource::SetVolume)
			.addFunction("GetVolume", &RAudioSource::GetVolume)
			.endClass()

			.beginClass<AudioClip>("AudioClip")
			.addProperty("Path", &AudioClip::mPath)
			.endClass()

			.beginClass<AudioManager>("AudioManager")
			.addFunction("LoadClip", &AudioManager::LoadClip)
			.addFunction("Play2D", &AudioManager::Play2D)
			.addFunction("Play3D", &AudioManager::Play3D)
			.endClass();
	}

	void RegisterPlayerPrefsAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<PlayerPrefs>("PlayerPrefs_")
			.addFunction("Save", &PlayerPrefs::Save)
			.addFunction("AddString", &PlayerPrefs::AddString)
			.addFunction("AddInt", &PlayerPrefs::AddInt)
			.addFunction("AddFloat", &PlayerPrefs::AddFloat)
			.addFunction("GetFloat", &PlayerPrefs::GetFloat)
			.addFunction("GetInt", &PlayerPrefs::GetInt)
			.addFunction("GetString", &PlayerPrefs::GetString)
			.endClass();
	}


	void RegisterPhysicsAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			// RigidBody
			.beginClass<RigidBody>("RigidBody")
			.addFunction("Move", &RigidBody::Move)
			.addFunction("GetVelocity", &RigidBody::GetVelocity)
			.addFunction("SetVelocity", &RigidBody::SetVelocity)
			.addFunction("SetAngels", &RigidBody::SetAngels)
			.addFunction("AddForce", &RigidBody::AddForce)
			.addFunction("AddCentralForce", &RigidBody::AddCentralForce)
			.addFunction("SetGravity", &RigidBody::SetGravity)
			.addFunction("SetGravityY", &RigidBody::SetGravityY)
			.addFunction("GetGravity", &RigidBody::GetGravity)
			.addFunction("GetScale", &RigidBody::GetScale)
			.addFunction("SetScale", &RigidBody::SetScale)
			.addFunction("SetFriction", &RigidBody::SetFriction)
			.addFunction("GetFriction", &RigidBody::GetFriction)
			.addFunction("SetBounciness", &RigidBody::SetBounciness)
			.addFunction("GetBounciness", &RigidBody::GetBounciness)
			.addFunction("Activate", &RigidBody::Activate)
			.addFunction("ApplyImpulse", &RigidBody::ApplyImpulse)
			.endClass()

			// RayHitInfo
			.beginClass<RayHitInfo>("RayHitInfo")
			.addConstructor<void(*) ()>()
			.addProperty("hasHit", &RayHitInfo::hasHit)
			.addProperty("distance", &RayHitInfo::distance)
			.addProperty("body", &RayHitInfo::body)
			.addProperty("point", &RayHitInfo::point)
			.addProperty("normal", &RayHitInfo::normal)
			.endClass()

			// PhysicsWorld
			.beginClass<PhysicsWorld>("PhysicsWorld")
			.addFunction("Raycast", &PhysicsWorld::Raycast)
			.addFunction("RayTest", &PhysicsWorld::RayTest)
			.addFunction("CheckSphere", &PhysicsWorld::CheckSphere)
			.addFunction("GetGravity", &PhysicsWorld::GetGravity)
			.addFunction("SetGravity", &PhysicsWorld::SetGravity)
			.endClass();
			;
	}

	template<typename T>
	void RegisterSceneAPI(lua_State* L)
	{
		luabridge::getGlobalNamespace(L)
			.beginClass<Transform>("Transform")
			.addFunction("GetPosition", &Transform::GetLocalPosition)
			.addFunction("SetPosition", &Transform::SetPosition)
			.addFunction("GetWorldPosition", &Transform::GetWorldPosition)
			.addFunction("SetWorldPosition", &Transform::SetWorldPosition)
			.addFunction("GetScale", &Transform::GetLocalScale)
			.addFunction("SetScale", &Transform::SetLocalScale)
			.addFunction("GetAngels", &Transform::GetEurlerAngels)
			.addFunction("SetAngels", &Transform::SetAngels)
			.addFunction("Forward", &Transform::forward)
			.addFunction("Right", &Transform::right)
			.addFunction("Up", &Transform::up)
			.addFunction("LookAt", &Transform::LookAt)
			.addFunction("Translate", &Transform::Translate)
			.endClass();

		// Time
		luabridge::getGlobalNamespace(L)
			.beginNamespace("Time")
			.addVariable("time", &this->time, false)
			.addVariable("deltaTime", &this->deltaTime, false)
			.addVariable("PhyDeltaTime", &this->PhyDeltaTime, false)
			.addVariable("frameRateLimit", &this->frameRateLimit)
			.addVariable("FPS", &this->FPS)
			.endNamespace();

		//Asset class
		luabridge::getGlobalNamespace(L)
			.beginClass<AssetPath>("AssetPath")
			.addConstructor<void(*) ()>()
			.addData("__type", &AssetPath::__type)
			.addFunction("Set", &AssetPath::SetPath)
			.addFunction("Get", &AssetPath::GetPath)
			.addFunction("GetType", &AssetPath::GetType)
			.endClass();

		luabridge::getGlobalNamespace(L)

			//SceneManager
			.beginClass<SceneManager>("SceneManager")
			.addFunction("SpawnEntity", &SceneManager::SpawnEntity)
			.addFunction("LoadScene", &SceneManager::LoadScene)
			.addFunction("QuitGame", &SceneManager::QuitGame)
			.addProperty("IsPlaying", &SceneManager::is_playing)
			.addFunction("GetEntity", &SceneManager::FindEntity)
			.addFunction("DestroyEntity", &SceneManager::DestroyEntity)
			.addFunction("GetRoot", &SceneManager::GetRoot)
			.addFunction("FindEntityWithTag", &SceneManager::FindEntityWithTag)
			.addFunction("CreateEntity", &SceneManager::CreateEntity<T>)
			.addFunction("Broadcast", &SceneManager::BroadcastScriptFunc)
			.endClass()

			// Entity
			.beginClass<Entity>("Entity")
			.addData("ID", &Entity::ID, false)
			.addProperty("Active", &Entity::Active)
			.addProperty("parent", &Entity::parent)
			.addProperty("name", &Entity::name)
			.addProperty("tag", &Entity::tag)
			.addProperty("transform", &Entity::transform)
			.addFunction("DestroySelf", &Entity::DestroySelf)
			.addFunction("DestroySelfIn", &Entity::DestroySelfIn)
			.addFunction("SetParent", &Entity::SetParent)
			.addFunction("GetChild", &Entity::GetChild)
			.addFunction("SetRotation", &Entity::SetRotation)
			.addFunction("SetTranslation", &Entity::SetTranslation)
			.addFunction("RotateY", &Entity::RotateY)
			.addFunction("Rotate", &Entity::Rotate)
			.addFunction("GetScriptIndex", &Entity::GetScriptInstance)
			.addFunction("GetChildWithTag", &Entity::GetChildWithTag)
			.addFunction("GetParentWithTag", &Entity::GetParentWithTag)
			.addFunction("Broadcast", &Entity::InvokeScriptFunc)
			.addProperty("ContactPos", &Entity::ContactPos)
			.addProperty("ContactNormal", &Entity::ContactNormal)

			.addFunction("GetParticleSystem", &Entity::GetComponent<ParticleSystem>)
			.addFunction("GetAudioSource", &Entity::GetComponent<RAudioSource>)
			.addFunction("GetRigidBody", &Entity::GetComponent<RigidBody>)
			.addFunction("GetPointLight", &Entity::GetComponent<PointLight>)
			.addFunction("GetSpotLight", &Entity::GetComponent<SpotLight>)
			.addFunction("GetDirectionalLight", &Entity::GetComponent<DirectionalLight>)
			.addFunction("GetCameraComponent", &Entity::GetComponent<CameraComponent>)
			.addFunction("GetBillboardComponent", &Entity::GetComponent<BillboardComponent>)
			.addFunction("GetReflectionProbe", &Entity::GetComponent<ReflectionProbe>)
			.addFunction("GetUIWidget", &Entity::GetComponent<UIWidget>)
			;
	}

	template<typename T>
	void RegisterAPI(lua_State* L)
	{
		RegisterAudioAPI(L);
		RegisterPhysicsAPI(L);
		RegisterRendererAPI(L);
		RegisterInputAPI(L);
		RegisterDebugAPI(L);
		RegisterMathAPI(L);
		RegisterUIAPI(L);
		RegisterPlayerPrefsAPI(L);
		RegisterSceneAPI<T>(L);
	}
}
