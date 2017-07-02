/// Copyright 2013-2017 - asm128
#include "multithread.h"
#include "nwol_debug.h"
#include "typeint.h"

#ifndef NWOL_NOISE_H__92374092374021784309287349023874
#define NWOL_NOISE_H__92374092374021784309287349023874

namespace nwol
{
	// I got this base random function code from http://libnoise.sourceforge.net/noisegen/index.html.
	static	double						noise1D				(uint32_t x, uint64_t noiseSeed=15731)																	{ x = (x<<13) ^ x; return (1.0 - ( (x * (x * x * noiseSeed + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);				}
	static	double						noiseNormal1D		(uint32_t x, uint64_t noiseSeed=15731)																	{ x = (x<<13) ^ x; return (1.0 - ( (x * (x * x * noiseSeed + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) *.5 + .5f;	}
	//------------------
	static	double						noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed=15731 )	{ x += (y * nWidth);							return noise1D(x, noiseSeed);		}
	static	double						noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed=15731 )	{ x += (y * nWidth + (z * nHeight * nWidth));	return noise1D(x, noiseSeed);		}
	static	double						noiseNormal2D		(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed=15731 )	{ x += (y * nWidth);							return noiseNormal1D(x, noiseSeed);	}
	static	double						noiseNormal3D		(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed=15731 )	{ x += (y * nWidth + (z * nHeight * nWidth));	return noiseNormal1D(x, noiseSeed);	}

#pragma pack(push, 1)
	struct SRandomGenerator {
				uint64_t					Seed				= 15731;
#if defined(NWOL_MTSUPPORT)
				refcount_t					Position			= 0;
#else
				uint32_t					Position			= 0;
#endif
				double						Value				= 0.0;

				void						Reset				()						{ Reset(Seed); }
				void						Reset				(uint64_t seed)			{
			Seed								= seed;
			Position							= 0;
			Value								= 0.0;
		}

		inline	double						Next				()						{
#if defined(NWOL_MTSUPPORT)
			return Value						= ::nwol::noiseNormal1D((uint32_t)NWOL_INTERLOCKED_INCREMENT(Position), Seed);
#else
			return Value						= ::nwol::noiseNormal1D(++Position, Seed);
#endif
		}
	};
#pragma pack(pop)

	// Based on noiseNormal1D() returns a value between 0 and 1.
	static double						randNoise			(uint64_t seed=15731)	{
		static	::nwol::SRandomGenerator		generator;
		if( generator.Seed != seed )
			generator.Reset(seed);
		return generator.Next();
	}
}

#endif // NWOL_NOISE_H__92374092374021784309287349023874
