#include "ParticleSystem.h"

IMPL_COMPONENT(ParticleSystem)
// ----------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
{
}
// ----------------------------------------------------------------------------
ParticleSystem::~ParticleSystem()
{
}
// ----------------------------------------------------------------------------
void ParticleSystem::Clear()
{
	std::cout << "ParticleSystem::Clear()" << std::endl;
	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteBuffers(1, &particles_offset_buffer);
	glDeleteVertexArrays(1, &quadVAO);

	delete[] g_particule_position_size_data;
	delete[] g_particule_color_data;
	delete[] g_particule_offset_data;
	mParticles.clear();
	mParticles.shrink_to_fit();
	initialized = false;
}
// ----------------------------------------------------------------------------
void ParticleSystem::SetMaxParticleCount(int newCount)
{
	if (MaxParticles == newCount) return;
	Clear();
	MaxParticles = newCount;
	Init();
}
// ----------------------------------------------------------------------------
unsigned int ParticleSystem::GetMaxParticleCount()
{
	return MaxParticles;
}
// ----------------------------------------------------------------------------
void ParticleSystem::Init()
{
	if (!initialized)
	{
		initialized = true;

		for (size_t i = 0; i < MaxParticles; i++)
		{
			mParticles.emplace_back();
		}

		g_particule_position_size_data = new GLfloat[MaxParticles * 4];
		g_particule_color_data = new GLubyte[MaxParticles * 4];
		g_particule_offset_data = new GLfloat[MaxParticles * 2];

		// The VBO containing the 4 vertices of the particles.
		// Thanks to instancing, they will be shared by all particles.
		float g_vertex_buffer_data[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &billboard_vertex_buffer);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), &g_vertex_buffer_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &particles_position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexAttribDivisor(2, 1);

		// The VBO containing the colors of the particles
		glGenBuffers(1, &particles_color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(unsigned char), (void*)0);
		glVertexAttribDivisor(3, 1);

		// The VBO containing the anim offsets
		glGenBuffers(1, &particles_offset_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_offset_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, particles_offset_buffer);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(4, 1);
	}
}
// ----------------------------------------------------------------------------
int ParticleSystem::FindUnusedParticle() {

	for (size_t i = LastUsedParticle; i < MaxParticles; i++) {
		if (mParticles[i].life <= 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (mParticles[i].life <= 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	if (Looping) return 0; // All particles are taken, override the first one
	return -1; 	// wait till one of the particles is dead.
}
// ----------------------------------------------------------------------------
void ParticleSystem::SortParticles() 
{
	std::sort(&mParticles[0], &mParticles[MaxParticles]);
}
// ----------------------------------------------------------------------------
void ParticleSystem::Update(float delta, glm::vec3 CameraPosition)
{
	if (!initialized)
		Init();

	delta *= SimulationSpeed;

	if (Emitting) {
		int newparticles = MaxParticles;
		if (Looping) {
			newparticles = (int)(delta * EmitteCount);
			if (newparticles > (int)(0.016f * EmitteCount))
				newparticles = (int)(0.016f * EmitteCount);
			//newparticles = MaxParticles / (int)StartLifeTime;
		}
		for (int i = 0; i < newparticles; i++) {
			int particleIndex = FindUnusedParticle();
			if (particleIndex == -1)
			{
				if(!Looping)	Emitting = false;
				continue;
			}

			mParticles[particleIndex].life = StartLifeTime;
			mParticles[particleIndex].pos = TargetPos;

			glm::vec3 maindir = Direction;
			glm::vec3 randomdir;

			randomdir = glm::vec3((float)Random::Range(-Spread, Spread),
				(float)Random::Range(-Spread, Spread),
				(float)Random::Range(-Spread, Spread));

			float startSpeed = Random::Range(StartSpeed.x, StartSpeed.y);
			maindir *= startSpeed;

			mParticles[particleIndex].speed = maindir + randomdir;

			mParticles[particleIndex].r = (unsigned char)(StartColor.r * 255);
			mParticles[particleIndex].g = (unsigned char)(StartColor.g * 255);
			mParticles[particleIndex].b = (unsigned char)(StartColor.b * 255);
			mParticles[particleIndex].a = (unsigned char)(StartColor.a * 255);

			mParticles[particleIndex].size = StartSize.x;//Random::Range(StartSize.x, StartSize.y);
		}
	}

	// Simulate all particles
	ParticlesCount = 0;
	for (size_t i = 0; i < MaxParticles; i++) {

		Particle& p = mParticles[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= delta;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				p.speed += GravityModifier * (float)delta * 0.5f;
				p.pos += p.speed * (float)delta;
				p.cameradistance = glm::length2(p.pos - CameraPosition);

				if (AddSizeOverTime != 0.0f)
					p.size += AddSizeOverTime * delta;

				if (anim_use)
				{
					if (p.lastUpdateTime >= (AnimSpeed * delta))
					{
						p.lastUpdateTime = 0.0f;
						if (p.animOffsetX >= 1) p.animOffsetX = 0;
						else p.animOffsetX += (1.0f / AnimSheetSize.x);

						if (p.animOffsetX >= 1)
						{
							if (p.animOffsetY >= 1 - (1.0f / AnimSheetSize.y)) p.animOffsetY = 0;
							else p.animOffsetY += (1.0f / AnimSheetSize.y);
						}
					}
					else p.lastUpdateTime += delta;
				}
				if (FadeOut > 0.0f && p.life < FadeOut * StartLifeTime)
					p.a = (unsigned char)(p.life / (FadeOut * StartLifeTime) * 255);

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

				g_particule_color_data[4 * ParticlesCount + 0] = p.r;
				g_particule_color_data[4 * ParticlesCount + 1] = p.g;
				g_particule_color_data[4 * ParticlesCount + 2] = p.b;
				g_particule_color_data[4 * ParticlesCount + 3] = p.a;

				g_particule_offset_data[2 * ParticlesCount + 0] = p.animOffsetX;
				g_particule_offset_data[2 * ParticlesCount + 1] = p.animOffsetY;
			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}

			ParticlesCount++;
		}
	}

	SortParticles();

	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

	if (anim_use)
	{
		glBindBuffer(GL_ARRAY_BUFFER, particles_offset_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 2, g_particule_offset_data);
	}
}
// ----------------------------------------------------------------------------