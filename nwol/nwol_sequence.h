#include "nwol_error.h"
#include "evaluation.h"

#ifndef NWOL_SEQUENCE_H_092374092378423
#define NWOL_SEQUENCE_H_092374092378423

namespace nwol
{
	template<typename _tElement>
	::nwol::error_t							sequence_compare						(const _tElement* sequenceA, const _tElement* sequenceB, uint32_t sequenceLength)											{
		uint32_t									currentChar								= 0;
		while(currentChar < sequenceLength)
			if(sequenceA[currentChar] != sequenceB[currentChar])
				return -1;
			else 
				++currentChar;

		return 0;
	}

	template<typename _tElement>
	::nwol::error_t							sequence_find							(const _tElement* source, uint32_t offset, uint32_t sourceSize, const _tElement* sequenceToFind, uint32_t sequenceLength)	{
		const uint32_t								indexStop								= sourceSize - sequenceLength;
		for(; offset < indexStop; ++offset) {
			if(0 == ::nwol::sequence_compare(&source[offset], sequenceToFind, sequenceLength))
				return offset;
		}
		return -1;
	}
} // nwol

#endif // NWOL_SEQUENCE_H_092374092378423