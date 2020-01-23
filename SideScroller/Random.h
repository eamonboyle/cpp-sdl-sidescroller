#pragma once
#include <random>
#include "Math.h"

class Random
{
public:
	static void Init();

	// seed the generator with the specified int
	static void Seed(unsigned int seed);

	// get a float between 0.0f and 1.0f
	static float GetFloat();

	// get a float from the specified range
	static float GetFloatRange(float min, float max);

	// get an int from the specified range
	static int GetIntRange(int min, int max);

	// get a random vector given the min/max bounds
	static Vector2 GetVector(const Vector2& min, const Vector2& max);
	static Vector3 GetVector(const Vector3& min, const Vector3& max);

private:
	static std::mt19937 sGenerator;
};

