/// Copyright 2013-2017 - asm128
#include "nwol_sync.h"
#include "nwol_debug.h"
#include "nwol_typeint.h"

#ifndef NWOL_NOISE_H__92374092374021784309287349023874
#define NWOL_NOISE_H__92374092374021784309287349023874

namespace nwol
{
	double								noise1D				(uint64_t x, uint64_t noiseSeed = 16381)																noexcept;
	double								noiseNormal1D		(uint64_t x, uint64_t noiseSeed = 16381)																noexcept;
			double						noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = 16381)	noexcept;
			double						noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = 16381)	noexcept;
			double						noiseNormal2D		(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = 16381)	noexcept;
			double						noiseNormal3D		(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = 16381)	noexcept;
	// Based on noiseNormal1D() returns a value between 0 and 1.
			double						randNoise			(uint64_t seed = 16381)																					noexcept;
}

#endif // NWOL_NOISE_H__92374092374021784309287349023874
