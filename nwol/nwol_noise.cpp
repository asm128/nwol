#include "nwol_random_generator.h"

// I got this base random function code from http://libnoise.sourceforge.net/noisegen/index.html.
double						::nwol::noise1D				(uint64_t x, uint64_t noiseSeed)																noexcept	{ x = (x << 13) ^ x; return (1.0 - ( (x * (x * x * noiseSeed + 786109ULL) + 1824261419ULL) & 0x7fffffffffffffffULL) / 4611686018427387903.0);			}
double						::nwol::noiseNormal1D		(uint64_t x, uint64_t noiseSeed)																noexcept	{ x = (x << 13) ^ x; return (1.0 - ( (x * (x * x * noiseSeed + 786109ULL) + 1824261419ULL) & 0x7fffffffffffffffULL) / 4611686018427387903.0) *.5 + .5f;	}
	//------------------
double						::nwol::noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth);							return noise1D(x, noiseSeed);		}
double						::nwol::noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return noise1D(x, noiseSeed);		}
double						::nwol::noiseNormal2D		(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth);							return noiseNormal1D(x, noiseSeed);	}
double						::nwol::noiseNormal3D		(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return noiseNormal1D(x, noiseSeed);	}
// Based on noiseNormal1D() returns a value between 0 and 1.
double						::nwol::randNoise			(uint64_t seed)																					noexcept	{
		static	::nwol::SRandomGenerator		generator;
		if( generator.Seed != seed )
			generator.Reset(seed);
		return generator.Next();
	}
