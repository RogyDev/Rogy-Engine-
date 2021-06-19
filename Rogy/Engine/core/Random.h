#ifndef RANDOM_NUMBER_H
#define RANDOM_NUMBER_H

namespace Random
{
	void SetRandomSeed(unsigned seed);

	unsigned GetRandomSeed();

	int Rand();

	int RangeI(int min, int max);
	float Range(float min, float max);

	float RandStandardNormal();
}
#endif