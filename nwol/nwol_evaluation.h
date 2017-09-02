/// Copyright 2016-2017 - asm128
#include "nwol_error.h"

#ifndef NWOL_EVALUATION_H_92364890278346982374
#define NWOL_EVALUATION_H_92364890278346982374

namespace nwol
{
	template <typename _tValue> static inline constexpr		const _tValue&			min						(const _tValue& a, const _tValue& b)													noexcept	{ return (a < b) ? a : b;											}
	template <typename _tValue> static inline constexpr		const _tValue&			max						(const _tValue& a, const _tValue& b)													noexcept	{ return (a > b) ? a : b;											}
	template <typename _tValue> static inline constexpr		bool					in_range				(const _tValue& valueToTest, const _tValue& rangeStart, const _tValue& rangeStop)		noexcept	{ return (valueToTest >= rangeStart) && (valueToTest < rangeStop);	}

	template<typename _tElement>
															::nwol::error_t			sequence_compare		(const _tElement* sequenceA, const _tElement* sequenceB, uint32_t sequenceLength)		noexcept	{
		uint32_t																			currentChar				= 0;
		while(currentChar < sequenceLength)
			if(sequenceA[currentChar] != sequenceB[currentChar])
				return -1;
			else 
				++currentChar;

		return 0;
	}

	template<typename _tElement>
															::nwol::error_t			sequence_find			(const _tElement* source, uint32_t offset, uint32_t sourceSize, const _tElement* sequenceToFind, uint32_t sequenceLength)	noexcept	{
		const uint32_t																		indexStop				= sourceSize - sequenceLength;
		for(; offset < indexStop; ++offset) {
			if(0 == ::nwol::sequence_compare(&source[offset], sequenceToFind, sequenceLength))
				return offset;
		}
		return -1;
	}

	static inline											::nwol::error_t			sz_compare				(const char* szSequenceA, const char* szSequenceB)										noexcept	{ return sequence_compare	(szSequenceA, szSequenceB, (uint32_t)::nwol::min(strlen(szSequenceA), strlen(szSequenceB)));	}
	static inline											::nwol::error_t			sz_find					(const char* szSource, uint32_t offset, const char* szToFind)							noexcept	{ return sequence_find		(szSource, offset, (uint32_t)strlen(szSource), szToFind, (uint32_t)strlen(szToFind));			}
} // namespace

#define true_if(expression)					((expression) ? true:false)		// returns true	 if the parameter evaluates to true
#define false_if(expression)				((expression) ? false:true)		// returns false if the parameter evaluates to true
#define one_if(expression)					((expression) ? 1:0)			// returns one   if the parameter evaluates to true
#define zero_if(expression)					((expression) ? 0:1)			// returns zero  if the parameter evaluates to true
//
#define is_one(your_int)					( 1 == (your_int))				// returns true if the parameter is equal to one
#define is_zero(your_int)					( 0 == (your_int))				// returns true if the parameter is equal to zero
#define is_onem(your_int)					(-1 == (your_int))				// returns true if the parameter is equal to one minus
		
#define iif(condition)						(condition) ?
#define gclamp(_value, _min, _max)			(::nwol::min((_max), ::nwol::max((_min), (_value))))

#endif // NWOL_EVALUATION_H_92364890278346982374
