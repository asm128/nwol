/// Copyright 2016-2017 - asm128
#include "nwol_debug.h"

#ifndef __EVALUATION_H__92364890278346982374__
#define __EVALUATION_H__92364890278346982374__

namespace nwol
{
	template <typename _tValue> static inline constexpr		const _tValue&	min			(const _tValue& a, const _tValue& b)													noexcept	{ return (a < b) ? a : b;								}
	template <typename _tValue> static inline constexpr		const _tValue&	max			(const _tValue& a, const _tValue& b)													noexcept	{ return (a > b) ? a : b;								}
	template <typename _tValue> static inline constexpr		bool			in_range	(const _tValue& value, const _tValue& rangeStart, const _tValue& rangeStop)				noexcept	{ return (value >= rangeStart) && (value < rangeStop);	}
} // namespace

#define true_if(expression)			((expression) ? true:false)		// returns true	 if the parameter evaluates to true
#define false_if(expression)		((expression) ? false:true)		// returns false if the parameter evaluates to true
#define one_if(expression)			((expression) ? 1:0)			// returns one   if the parameter evaluates to true
#define zero_if(expression)			((expression) ? 0:1)			// returns zero  if the parameter evaluates to true
//
#define is_one(your_int)			( 1 == (your_int))				// returns true if the parameter is equal to one
#define is_zero(your_int)			( 0 == (your_int))				// returns true if the parameter is equal to zero
#define is_onem(your_int)			(-1 == (your_int))				// returns true if the parameter is equal to one minus
		
#define iif(condition)				(condition) ?
#define gclamp(_value, _min, _max)	(::nwol::min((_max), ::nwol::max((_min), (_value))))

#endif // __EVALUATION_H__92364890278346982374__
