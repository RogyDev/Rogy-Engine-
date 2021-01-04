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

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(std::string("1.0"));
		ar(Emitting);
		ar(PlayOnStart);
		ar(Looping);
		ar(StartLifeTime);
		ar(StartColor.x, StartColor.y, StartColor.z, StartColor.w);
		ar(StartSpeed.x, StartSpeed.y);
		ar(StartSize.x, StartSize.y);
		ar(SimulationSpeed);
		ar(GravityModifier.x, GravityModifier.y, GravityModifier.z);
		ar(Spread);
		ar(EmitteCount);
		ar(AddSizeOverTime);
		ar(UseTexTransparent);
		ar(FadeOut);
		ar(anim_use);
		ar(AnimSheetSize.x, AnimSheetSize.y);
		ar(AnimSpeed);
		ar(MaxParticles);
		if (mTexture != nullptr)
			tex_path = mTexture->getTexName();
		ar(tex_path);
		ar(ReceiveShadows);
	}

	template <class Archive>
	void serializeLoad(Archive & ar)
	{
		std::string ver;
		ar(ver);
		ar(Emitting);
		ar(PlayOnStart);
		ar(Looping);
		ar(StartLifeTime);
		ar(StartColor.x, StartColor.y, StartColor.z, StartColor.w);
		ar(StartSpeed.x, StartSpeed.y);
		ar(StartSize.x, StartSize.y);
		ar(SimulationSpeed);
		ar(GravityModifier.x, GravityModifier.y, GravityModifier.z);
		ar(Spread);
		ar(EmitteCount);
		ar(AddSizeOverTime);
		ar(UseTexTransparent);
		ar(FadeOut);
		ar(anim_use);
		ar(AnimSheetSize.x, AnimSheetSize.y);
		ar(AnimSpeed);
		unsigned int maxP = 1000;
		ar(maxP);
		SetMaxParticleCount(maxP);
		ar(tex_path);
		ar(ReceiveShadows);
	}

private:
	bool initialized = false;
	int currentFreme;
	unsigned int MaxParticles = 1000;
	std::vector<Particle> mParticles;
};

#endif // PARTICAL_SYSYTEM_H