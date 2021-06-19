#include "AudioClip.h"

AudioClip::AudioClip()
{
}

AudioClip::~AudioClip()
{
}

bool AudioClip::LoadClip(std::string path)
{
	mPath = path;
	SoLoud::result rs = gWave.load(path.c_str());
	if (rs == SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED || rs == SoLoud::SOLOUD_ERRORS::FILE_NOT_FOUND
		|| rs == SoLoud::SOLOUD_ERRORS::INVALID_PARAMETER)
		return false;

	return true;
}
