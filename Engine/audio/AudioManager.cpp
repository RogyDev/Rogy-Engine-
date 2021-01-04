#include "AudioManager.h"
// ---------------------------------------------------------------
AudioManager::AudioManager()
{
}
// ---------------------------------------------------------------
AudioManager::~AudioManager()
{
}
// ---------------------------------------------------------------
AudioClip * AudioManager::LoadClip(std::string path)
{
	for (size_t i = 0; i < m_clips.size(); i++)
		if (m_clips[i]->mPath == path)
			return m_clips[i];

	AudioClip* clip = new AudioClip();
	if (clip->LoadClip(path))
	{
		m_clips.push_back(clip);
		return clip;
	}
	
	delete clip;
	return nullptr;
}
// ---------------------------------------------------------------
RAudioSource * AudioManager::AddComponent(unsigned int ent_id)
{
	RAudioSource* auD = mSources.AddComponent(ent_id);
	auD->gSoloud = &gSoloud;
	return auD;
}
// ---------------------------------------------------------------
void AudioManager::Init()
{
	// initialize SoLoud.
	gSoloud.init();
}
// ---------------------------------------------------------------
void AudioManager::Clear()
{
	// Clean up SoLoud
	gSoloud.deinit();
}
// ---------------------------------------------------------------
void AudioManager::Update()
{
	gSoloud.update3dAudio();
	mSources.ClearRemovedComponents();
}
// ---------------------------------------------------------------
void AudioManager::Play2D(AudioClip * aclip, float volume)
{
	if (aclip == nullptr) return;
	gSoloud.play(aclip->gWave, volume);
}
// ---------------------------------------------------------------
void AudioManager::Play3D(AudioClip * aclip, glm::vec3 pos, float min, float max, float volume)
{
	if (aclip == nullptr) return;
	int shandle = gSoloud.play3d(aclip->gWave, pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, volume);
	gSoloud.set3dSourceAttenuation(shandle, SoLoud::AudioSource::LINEAR_DISTANCE, 1.0f);
	gSoloud.set3dSourceMinMaxDistance(shandle, min, max);
	gSoloud.update3dAudio();
}
// ---------------------------------------------------------------
void AudioManager::SetListenerPosition(glm::vec3 pos)
{
	if (pos.x != px || pos.y != py || pos.z != pz)
	{
		px = pos.x;
		py = pos.y;
		pz = pos.z;
		update3dinfo = true;
		gSoloud.set3dListenerPosition(px, py, pz);
	}
}
// ---------------------------------------------------------------
void AudioManager::SetListenerUp(glm::vec3 pos)
{
	if (pos.x != ux || pos.y != uy || pos.z != uz)
	{
		ux = pos.x;
		uy = pos.y;
		uz = pos.z;
		update3dinfo = true;
		gSoloud.set3dListenerUp(ux, uy, uz);
	}
}
// ---------------------------------------------------------------
void AudioManager::SetListenerForward(glm::vec3 pos)
{
	if (pos.x != fx || pos.y != fy || pos.z != fz)
	{
		fx = pos.x;
		fy = pos.y;
		fz = pos.z;
		update3dinfo = true;
		gSoloud.set3dListenerAt(fx, fy, fz);
	}
}
// ---------------------------------------------------------------
void AudioManager::PauseAll()
{
	gSoloud.stopAll();
}
// ---------------------------------------------------------------