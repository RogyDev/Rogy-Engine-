#include "AudioSource.h"
IMPL_COMPONENT(RAudioSource)

// ---------------------------------------------------------------
RAudioSource::RAudioSource()
{}
RAudioSource::~RAudioSource()
{
	Stop();
}
// ---------------------------------------------------------------
void RAudioSource::SetPosition(glm::vec3 mpos)
{
	if (pos == mpos) return;
	pos = mpos;
	gSoloud->set3dSourcePosition(handle, pos.x, pos.y, pos.z);
}
// ---------------------------------------------------------------
void RAudioSource::Play()
{
	if (mClip != nullptr)
	{
		if (!Overlap && IsPlaying())
			Stop();
		is_playing = true;

		if(Is2D)
			handle = gSoloud->play(mClip->gWave, volume);
		else 
		{
			handle = gSoloud->play3d(mClip->gWave, pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, volume);
			gSoloud->set3dSourceAttenuation(handle, SoLoud::AudioSource::LINEAR_DISTANCE, 1.0f);
			gSoloud->set3dSourceMinMaxDistance(handle, minD, maxD);
		}
		gSoloud->setLooping(handle, Looping);
		gSoloud->setFilterParameter(handle, 2, 0, EchoFilter);
	}
}
// ---------------------------------------------------------------
void RAudioSource::Stop()
{
	if (mClip != nullptr)
	{
		is_playing = false;
		gSoloud->stop(handle);
	}
}
// ---------------------------------------------------------------
bool RAudioSource::IsPlaying()
{
	return is_playing;
}
void RAudioSource::SetClip(AudioClip * clip)
{
	mClip = clip;
}
// ---------------------------------------------------------------
void RAudioSource::SetMinDistance(float min_d)
{
	minD = min_d;
	gSoloud->set3dSourceMinMaxDistance(handle, minD, maxD);
}
// ---------------------------------------------------------------
void RAudioSource::SetMaxDistance(float max_d)
{
	maxD = max_d;
	gSoloud->set3dSourceMinMaxDistance(handle, minD, maxD);
}
// ---------------------------------------------------------------
float RAudioSource::GetMinDistance()
{
	return minD;
}
// ---------------------------------------------------------------
float RAudioSource::GetMaxDistance()
{
	return maxD;
}
// ---------------------------------------------------------------
float RAudioSource::GetVolume()
{
	return volume;
}
// ---------------------------------------------------------------
void RAudioSource::SetVolume(float v)
{
	volume = v;
	gSoloud->setVolume(handle, volume);
}
// ---------------------------------------------------------------
void RAudioSource::SetLooping(bool v)
{
	if (Looping == v) return;
	Looping = v;
	gSoloud->setLooping(handle, Looping);
}
// ---------------------------------------------------------------
bool RAudioSource::GetLooping()
{
	//gSoloud->getLooping(handle);
	return Looping;
}
// ---------------------------------------------------------------
void RAudioSource::SetEchoFilter(float v)
{
	if (EchoFilter == v) return;
	EchoFilter = v;
	gSoloud->setFilterParameter(handle, 2, 0, EchoFilter);
}
// ---------------------------------------------------------------
float RAudioSource::GetEchoFilter()
{
	return EchoFilter;
}
// ---------------------------------------------------------------
