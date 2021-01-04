#ifndef RAUDIO_MANAGER_H
#define RAUDIO_MANAGER_H

#include "soloud.h"
#include "soloud_thread.h"
#include "soloud_wav.h"
#include "AudioSource.h"

class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	SoLoud::Soloud gSoloud; // SoLoud engine
	SoLoud::EchoFilter gEchoFilter;

	std::vector<AudioClip*> m_clips;
	AudioClip* LoadClip(std::string path);

	ComponentArray<RAudioSource> mSources;
	RAudioSource* AddComponent(unsigned int ent_id);

	void Init();
	void Clear();
	void Update();

	void Play2D(AudioClip* aclip, float volume = 1.0f);
	void Play3D(AudioClip* aclip, glm::vec3 pos, float min = 1.0f, float max = 1000.0f, float volume = 1.0f);

	void SetListenerPosition(glm::vec3 pos);
	void SetListenerUp(glm::vec3 pos);
	void SetListenerForward(glm::vec3 pos);
	void PauseAll();

private:
	float px, py, pz;
	float ux, uy, uz;
	float fx, fy, fz;
	bool update3dinfo = false;
};

#endif // ! RAUDIO_MANAGER_H
