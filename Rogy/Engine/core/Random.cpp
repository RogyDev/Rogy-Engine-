#include "Random.h"

namespace Random {
	static unsigned randomSeed = 1;

	void SetRandomSeed(unsigned seed)
	{
		randomSeed = seed;
	}

	unsigned GetRandomSeed()
	{
		return randomSeed;
	}

	int Rand()
	{
		randomSeed = randomSeed * 214013 + 2531011;
		return (randomSeed >> 16) & 0x7FFF; //32767;
	}

	int RangeI(int min, int max)
	{
		return min + (Rand() % (max - min + 1));
	}

	float Range(float min, float max)
	{
		return (float)(RangeI((int)min * 1000, (int)max * 1000)) / 1000.0f;
	}

	float RandStandardNormal()
	{
		float val = 0.0f;
		for (int i = 0; i < 12; i++)
			val += Rand() / 32768.0f;
		val -= 6.0f;

		// Now val is approximatly standard normal distributed
		return val;
	}
}