#ifndef R_AUDIO_SOURCE_MANAGER_H
#define R_AUDIO_SOURCE_MANAGER_H

#include "soloud.h"
#include "soloud_thread.h"
#include "soloud_wav.h"
#include "../scene/RComponent.h"
#include "AudioClip.h"
#include "soloud_echofilter.h"
#include <glm\glm.hpp>

class RAudioSource : public Component
{
	BASE_COMPONENT()
public:
	RAudioSource();
	~RAudioSource();

	// SoLoud engine
	SoLoud::Soloud* gSoloud = nullptr;
	AudioClip* mClip = nullptr;

	glm::vec3 pos;

	void SetPosition(glm::vec3 mpos);
	void Play();
	void Stop();
	bool IsPlaying();
	void SetClip(AudioClip* clip);

	void SetMinDistance(float min_d);
	void SetMaxDistance(float max_d);
	float GetMinDistance();
	float GetMaxDistance();
	float GetVolume();
	void SetVolume(float v);
	void SetLooping(bool v);
	bool GetLooping();
	bool PlayOnStart = true;

	void SetEchoFilter(float v);
	float GetEchoFilter();

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(enabled);
		ar(PlayOnStart);
		ar(GetMinDistance());
		ar(GetMaxDistance());
		ar(GetVolume());
		ar(GetLooping());
		if (mClip != nullptr)
			clip_path = mClip->mPath;
		ar(clip_path);
		ar(EchoFilter);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		ar(enabled);
		ar(PlayOnStart);
		float mindd, maxdd, vol;
		bool loopin;
		ar(mindd);
		ar(maxdd);
		ar(vol);
		ar(loopin);
		SetMinDistance(mindd);
		SetMaxDistance(maxdd);
		SetVolume(vol);
		SetLooping(loopin);
		ar(clip_path);
		ar(EchoFilter);
	}
	std::string clip_path;
private:
	unsigned int handle;
	float minD = 5.0f, maxD = 15.0f;
	float volume = 1.0f;
	bool Looping = false;
	float EchoFilter = 0.0f;
};

#endif // ! R_AUDIO_SOURCE_MANAGER_H
