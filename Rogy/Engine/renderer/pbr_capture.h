#ifndef R_PBR_CAPTURE_H
#define R_PBR_CAPTURE_H

// Container object for holding all per-environment specific pre-computed PBR data.
struct PBRCapture
{
	unsigned int Irradiance;
	unsigned int Prefiltered;
	bool created = false;
};

#endif 