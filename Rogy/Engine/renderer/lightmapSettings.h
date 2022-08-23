#ifndef LIGHTMAP_SETTINGS_H
#define LIGHTMAP_SETTINGS_H


struct LightmapSettings
{
	int resolution;
	int quality;
	float MaxDistance;
	int Passes;
	float threshold;
	std::string savePath;

	LightmapSettings()
	{
		resolution = 200;
		quality = 32;
		MaxDistance = 20.0f;
		Passes = 6;
		threshold = 0.01f;
		savePath = "";
	}
};

#endif