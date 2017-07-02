/// Copyright 2010-2017 - asm128
#include "evaluation.h"
#include "typeint.h"
#include <cmath>

#ifndef NWOL_MATH_H__92836409283642038462309846
#define NWOL_MATH_H__92836409283642038462309846

namespace nwol
{
	// Commonly used constants
	static											const double		math_pi					= 3.1415926535897932384626433832795;	// Pi
	static											const double		math_pi2				= 1.5707963267948966192313216916398;	// Pi / 2
	static											const double		math_2pi				= 6.2831853071795864769252867665590;	// 2 * Pi
	static											const double		math_pi_180				= math_pi/180.0;						// Pi / 180.0

	template<typename _tNumber>	inline constexpr	_tNumber			clamp					(const _tNumber _value, const _tNumber _min, const _tNumber _max)	noexcept	{ return ::nwol::min(_max, ::nwol::max(_min, _value));										}
	template<typename _tNumber>	inline constexpr	double				sqrt_safe				(const _tNumber _valueSquared)										noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;											}
	template<typename _tNumber>	inline constexpr	_tNumber			interpolate_linear		(const _tNumber a, const _tNumber b, const double factor)			noexcept	{ return (_tNumber)(b * factor + a * (1.0 - factor));										}
	template<typename _tNumber>	inline constexpr	_tNumber			interpolate_cosine		(const _tNumber a, const _tNumber b, const double x)				noexcept	{ return interpolate_linear(a, b, (1.0 - cos(x * math_pi)) * .5);							}
	template<typename _tNumber>	inline constexpr	_tNumber			mix						(const _tNumber a, const _tNumber b, const double factor)			noexcept	{ return interpolate_linear(a, b, factor);													}
	// 
	static inline									uint64_t			powui					(const uint32_t	base, const uint32_t	exponent)					noexcept	{ uint64_t	result=1; for( uint32_t i=0; i<exponent; ++i ) result *= base; return result;	}
	static inline									int64_t				powi					(const int32_t	base, const uint32_t	exponent)					noexcept	{ int64_t	result=1; for( uint32_t i=0; i<exponent; ++i ) result *= base; return result;	}
	static inline									float64_t			powd					(const int32_t	base, const int32_t		exponent)					noexcept	{ float64_t result=1;
		if( exponent >= 0 )
			for( int32_t i=0; i<exponent; ++i )
				result																*= base;	
		else
			for( int32_t i=0, count=-exponent; i<count; ++i )
				result																/= base;	
		return result;
	}

	struct	SPairSinCos								{ double Sin, Cos;	};	
	
	static inline									SPairSinCos			getSinCos				(const double theta)												noexcept	{ return {sin(theta), cos(theta)};															}
	static inline constexpr							bool				isPowerOfTwo			(const int num)														noexcept	{ return !(num & (num - 1));																}

} // namespace

#endif // NWOL_MATH_H__92836409283642038462309846
