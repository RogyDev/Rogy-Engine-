#ifndef ROGY_PROJECT_SETTINGS_H
#define ROGY_PROJECT_SETTINGS_H

#include <string>
#include <glm\glm.hpp>
#include <vector>

struct RProjectSettings
{
	// Game Info
	// -------------------------------
	std::string GameName;
	std::string CreatorName;
	std::string Version;
	std::string MainScenePath;
	bool DefaultFullScreen;
	int ResolutionWeight;
	int ResolutionHeight;

	// Graphics quality
	// -------------------------------
	int CaptureResulotion;
	int ReflectionProbeLimit;
	bool EnableShadows;
	float ShadowDistance;
	int CascadesCount;
	int CascadedShadowMapsResolution;
	float CascadeSplits[3];
	bool useDepthPrePass;
	bool useInstancing;
	bool useInstancingForShadows;

	int SpotShadowsResolution;
	int PointShadowResolution;
	int SpotShadowsLimit;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(string("1.0"));
		ar(GameName);
		ar(CreatorName);
		ar(Version);
		ar(MainScenePath);
		ar(DefaultFullScreen);
		ar(ResolutionWeight);
		ar(ResolutionHeight);

		ar(EnableShadows);
		ar(CascadesCount);
		ar(ShadowDistance);
		ar(CascadeSplits[0]);
		ar(CascadeSplits[1]);
		ar(CascadeSplits[2]);
		ar(CascadedShadowMapsResolution);
		ar(SpotShadowsResolution);
		ar(PointShadowResolution);
		ar(SpotShadowsLimit);

		ar(useDepthPrePass);
		ar(useInstancing);
		ar(useInstancingForShadows);
		ar(CaptureResulotion);
		ar(ReflectionProbeLimit);
	}

	RProjectSettings()
	{
		GameName = "RogyGame";
		CreatorName = "YourName";
		Version = "1.0";
		MainScenePath = "";
		DefaultFullScreen = false;
		ResolutionWeight = 1280;
		ResolutionHeight = 800;

		EnableShadows = true;
		CascadesCount = 2;
		ShadowDistance = 60.0f;
		CascadedShadowMapsResolution = 1024;
		SpotShadowsResolution = 256;
		PointShadowResolution = 256;
		SpotShadowsLimit = 9;

		useDepthPrePass = true;
		useInstancing = true;
		useInstancingForShadows = true;

		CascadeSplits[0] = 0.1f;
		CascadeSplits[1] = 0.4f;
		CascadeSplits[2] = 1.0f;

		CaptureResulotion = 128;
		ReflectionProbeLimit = 8;
	}
};

#endif // ! ROGY_PROJECT_SETTINGS_H