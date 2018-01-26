#include "nwol_noise.h"

#ifndef NWOL_RANDOM_GENERATOR_H_29843293874
#define NWOL_RANDOM_GENERATOR_H_29843293874

namespace nwol 
{
#pragma pack(push, 1)
	struct SRandomGenerator {
				uint64_t					Seed				= 16381;
#if defined(NWOL_MTSUPPORT)
				refcount_t					Position			= 0;
#else
				uint32_t					Position			= 0;
#endif
				double						Value				= 0.0;

				void						Reset				()																										noexcept	{ Reset(Seed); }
				void						Reset				(uint64_t seed)																							noexcept	{
			Seed								= seed;
			Position							= 0;
			Value								= 0.0;
		}

		inline	double						Next				()																										noexcept	{
#if defined(NWOL_MTSUPPORT)
			return Value						= ::nwol::noiseNormal1D(nwol_sync_increment(Position), Seed);
#else
			return Value						= ::nwol::noiseNormal1D(++Position, Seed);
#endif
		}
	};
#pragma pack(pop)
} // namespace

#endif // NWOL_RANDOM_GENERATOR_H_29843293874
