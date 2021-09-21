#pragma once

#include <stdlib.h>

#define IEEE_754_MANTISSA 0x007fffff
#define IEEE_754_ONE 0x3f800000

namespace GLCore {

	class Random
	{

	private:
		static unsigned int seed;

		union FloatToInt
		{

			int intBytes;
			float floatBytes;

		};

	private:
		static inline unsigned int hash(int value);

	public:
		static inline void SetSeed(unsigned int seed);
		static inline int Int();
		static inline float Float();

	};

	inline unsigned int Random::hash(int value)
	{
		// Taken from the finalization
		// of the MurmurHash function
		value ^= value >> 16;
		value *= 0x85ebca6bu;
		value ^= value >> 13;
		value *= 0xc2b2ae35u;
		value ^= value >> 16;

		return value;
	}

	inline void Random::SetSeed(unsigned int seed)
	{

		Random::seed = seed;
		srand(seed);

	}
	inline int Random::Int()
	{

		return rand();
	
	}
	inline float Random::Float()
	{

		int intRand = Random::Int();
		unsigned int hash = Random::hash(intRand);

		Random::FloatToInt random;
		random.intBytes = (hash & IEEE_754_MANTISSA) | IEEE_754_ONE;

		return random.floatBytes - 1.0f;
	}
}

#undef IEEE_754_MANTISSA
#undef IEEE_754_ONE