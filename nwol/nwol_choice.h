/// Copyright 2015-2017 - asm128
#include "nwol_noise.h"
#include "nwol_array.h"

#ifndef __CHOICE_H_9823423__
#define __CHOICE_H_9823423__

namespace nwol
{
	uint32_t								weightedChoice							(uint32_t weightCount, const uint32_t* weights, uint64_t Seed)	{
		::nwol::array_pod<uint64_t>					totals									(weightCount);
		uint64_t									running_total							= 0;

		for( uint32_t i=0; i<weightCount; ++i ) {
			running_total							+= weights[i];
			totals[i]								= running_total;
		}

		double										noise									= ::nwol::noiseNormal1D((uint32_t)Seed);
		uint64_t									rnd										= (uint64_t)(noise*running_total);
		for( uint32_t i=0; i<totals.size(); i++ )
			if( rnd < totals[i] ) 
				return i;

		return -1;
	}
}

#endif // __CHOICE_H_9823423__