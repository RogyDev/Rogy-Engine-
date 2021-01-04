#ifndef R_AUDIO_CLIP_MANAGER_H
#define R_AUDIO_CLIP_MANAGER_H

#include "soloud.h"
#include "soloud_thread.h"
#include "soloud_wav.h"
#include <string>

class AudioClip
{
public:
	AudioClip();
	~AudioClip();

	SoLoud::Wav gWave;
	std::string mPath;

	bool LoadClip(std::string path);
};

#endif // ! RAUDIO_MANAGER_H
