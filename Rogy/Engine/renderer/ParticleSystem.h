#ifndef PARTICAL_SYSYTEM_H
#define PARTICAL_SYSYTEM_H

#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <gl\glew.h>
#include "../core/Random.h"
#include "../shading/texture.h"

// CPU representation of a particle
struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float animOffsetX, animOffsetY;
	float lastUpdateTime;
	// Remaining life of the particle. if <0 : dead and unused.
	float life;
	// *Squared* distance to the camera. if dead : -1.0f
	float cameradistance;

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticleSystem : public Component
{
	BASE_COMPONENT()
public:
	ParticleSystem();
	~ParticleSystem();

	// Component Properties
	// -------------------------------------------------
	bool Emitting = true;
	bool PlayOnStart = true;
	bool Looping = true;

	glm::vec4 StartColor = glm::vec4(1.0f);
	float StartLifeTime = 5.0f;
	glm::vec2 StartSpeed = glm::vec2(5.0f, 5.0f);
	glm::vec2 StartSize = glm::vec2(1.0f, 1.0f);
	float SimulationSpeed = 1.0f;
	glm::vec3 GravityModifier = glm::vec3(0.0f, -9.81f, 0.0f);
	float Spread = 5.0f;
	bool OneDirection = true;
	float EmitteCount = 1000.0f;
	float AddSizeOverTime = 0.0f;

	// Rendering
	Texture* mTexture = nullptr;
	bool UseTexTransparent = true;
	float FadeOut = 0.1f;
	bool ReceiveShadows = false;

	// Animation Sheet Texture
	bool anim_use = false;
	glm::vec2 AnimSheetSize = glm::vec2(1.0f, 1.0f);
	float AnimSpeed = 20.0f;

	// Particle System stuff
	// -------------------------------------------------
	glm::vec3 TargetPos = glm::vec3(0.0f);
	glm::vec3 Direction;
	std::string tex_path = "non";

	void Init();
	int FindUnusedParticle();
	void SortParticles();
	void Update(float delta, glm::vec3 CameraPosition);
	void Clear();
	void SetMaxParticleCount(int newCount);
	unsigned int GetMaxParticleCount();

	void Emitte()
	{
		Emitting = true;
	}
	void Stop()
	{
		Emitting = false;
	}

	int LastUsedParticle = 0;
	int ParticlesCount = 0;
	GLuint quadVAO;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	GLuint particles_offset_buffer;

	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	GLfloat* g_particule_offset_data;

	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "ParticleSystem" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "Emitting" << YAML::Value << Emitting;
		out << YAML::Key << "PlayOnStart" << YAML::Value << PlayOnStart;
		out << YAML::Key << "Looping" << YAML::Value << Looping;
		out << YAML::Key << "StartLifeTime" << YAML::Value << StartLifeTime;
		out << YAML::Key << "StartColor"; RYAML::SerVec4(out, StartColor);
		out << YAML::Key << "StartSpeed"; RYAML::SerVec2(out, StartSpeed);
		out << YAML::Key << "StartSize"; RYAML::SerVec2(out, StartSize);
		out << YAML::Key << "SimulationSpeed" << YAML::Value << SimulationSpeed;
		out << YAML::Key << "GravityModifier"; RYAML::SerVec3(out, GravityModifier);
		out << YAML::Key << "Spread" << YAML::Value << Spread;
		out << YAML::Key << "EmitteCount" << YAML::Value << EmitteCount;
		out << YAML::Key << "AddSizeOverTime" << YAML::Value << AddSizeOverTime;
		out << YAML::Key << "UseTexTransparent" << YAML::Value << UseTexTransparent;
		out << YAML::Key << "FadeOut" << YAML::Value << FadeOut;
		out << YAML::Key << "anim_use" << YAML::Value << anim_use;
		out << YAML::Key << "AnimSheetSize"; RYAML::SerVec2(out, AnimSheetSize);
		out << YAML::Key << "AnimSpeed" << YAML::Value << AnimSpeed;
		out << YAML::Key << "MaxParticles" << YAML::Value << MaxParticles;
		if (mTexture != nullptr)	tex_path = mTexture->getTexPath();
		out << YAML::Key << "tex_path" << YAML::Value << tex_path;
		out << YAML::Key << "ReceiveShadows" << YAML::Value << ReceiveShadows;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		Emitting = data["Emitting"].as<bool>();
		PlayOnStart = data["PlayOnStart"].as<bool>();
		Looping = data["Looping"].as<bool>();
		StartLifeTime = data["StartLifeTime"].as<float>();
		StartColor = RYAML::GetVec4(data["StartColor"]);
		StartSize = RYAML::GetVec2(data["StartSize"]);
		SimulationSpeed = data["SimulationSpeed"].as<bool>();
		GravityModifier = RYAML::GetVec3(data["AnimSheetSize"]);
		Spread = data["Spread"].as<float>();
		EmitteCount = data["EmitteCount"].as<float>();
		AddSizeOverTime = data["AddSizeOverTime"].as<float>();
		UseTexTransparent = data["UseTexTransparent"].as<bool>();
		FadeOut = data["FadeOut"].as<float>();
		anim_use = data["anim_use"].as<bool>();
		AnimSheetSize = RYAML::GetVec2(data["AnimSheetSize"]);
		AnimSpeed = data["AnimSpeed"].as<float>();
		unsigned int maxP = data["MaxParticles"].as<unsigned int>();
		SetMaxParticleCount(maxP);
		tex_path = data["tex_path"].as<std::string>();
		ReceiveShadows = data["ReceiveShadows"].as<bool>();
	}

private:
	bool initialized = false;
	int currentFreme;
	unsigned int MaxParticles = 1000;
	std::vector<Particle> mParticles;
};

#endif // PARTICAL_SYSYTEM_H