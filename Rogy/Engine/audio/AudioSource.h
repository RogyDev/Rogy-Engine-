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
	bool Overlap = false;

	void SetEchoFilter(float v);
	float GetEchoFilter();

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "RAudioSource" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "PlayOnStart" << YAML::Value << PlayOnStart;
		out << YAML::Key << "GetMinDistance" << YAML::Value << GetMinDistance();
		out << YAML::Key << "GetMaxDistance" << YAML::Value << GetMaxDistance();
		out << YAML::Key << "GetVolume" << YAML::Value << GetVolume();
		out << YAML::Key << "GetLooping" << YAML::Value << GetLooping();
		if (mClip != nullptr)	clip_path = mClip->mPath;
		out << YAML::Key << "clip_path" << YAML::Value << clip_path;
		out << YAML::Key << "EchoFilter" << YAML::Value << EchoFilter;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		PlayOnStart = data["PlayOnStart"].as<bool>();
		float mindd, maxdd, vol;
		bool loopin;
		mindd = data["GetMinDistance"].as<bool>();
		maxdd = data["GetMaxDistance"].as<float>();
		vol = data["GetVolume"].as<float>();
		loopin = data["GetLooping"].as<bool>();
		clip_path = data["clip_path"].as<std::string>();
		EchoFilter = data["EchoFilter"].as<float>();
	}

	std::string clip_path;
private:
	unsigned int handle;
	float minD = 5.0f, maxD = 15.0f;
	float volume = 1.0f;
	bool Looping = false;
	float EchoFilter = 0.0f;
	bool is_playing = false;
};

#endif // ! R_AUDIO_SOURCE_MANAGER_H
