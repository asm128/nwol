#include "evaluation.h"
#include "typeint.h"
#include <cmath>

#ifndef __NWOL_MATH_H__92836409283642038462309846__
#define __NWOL_MATH_H__92836409283642038462309846__

namespace nwol
{
	// Commonly used constants
	static											const double		math_pi				= 3.1415926535897932384626433832795;	// Pi
	static											const double		math_pi2			= 1.5707963267948966192313216916398;	// Pi/2
	static											const double		math_2pi			= 6.2831853071795864769252867665590;	// 2*Pi
	static											const double		math_pi_180			= math_pi/180.0;						// Pi/180.0

	template<typename _tNumber>	inline constexpr	_tNumber			clamp				(_tNumber _value, _tNumber _min, _tNumber _max)		noexcept	{ return ::nwol::min(_max, ::nwol::max(_min, _value));				}
	template<typename _tNumber>	inline constexpr	double				sqlen_safe			(_tNumber _valueSquared)							noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;					}
	template<typename _tNumber>	inline constexpr	_tNumber			interpolate_linear	(_tNumber a, _tNumber b, double factor)				noexcept	{ return (_tNumber)(b * factor + a * (1.0 - factor));				}
	template<typename _tNumber>	inline constexpr	_tNumber			interpolate_cosine	(_tNumber a, _tNumber b, double x)					noexcept	{ return interpolate_linear(a, b, (1.0 - cos(x * math_pi)) * .5);	}
	template<typename _tNumber>	inline constexpr	_tNumber			mix					(_tNumber a, _tNumber b, double factor)				noexcept	{ return interpolate_linear(a, b, factor);							}

	// 
	static inline									uint64_t			powui				(uint32_t	base, uint32_t	exponent)				noexcept	{ uint64_t	result=1; for( uint32_t i=0; i<exponent; ++i ) result *= base; return result; }
	static inline									int64_t				powi				(int32_t	base, uint32_t	exponent)				noexcept	{ int64_t	result=1; for( uint32_t i=0; i<exponent; ++i ) result *= base; return result; }
	static inline									float64_t			powd				(int32_t	base, int32_t	exponent)				noexcept	{ float64_t result=1;
		if( exponent >= 0 )
			for( int32_t i=0; i<exponent; ++i )
				result *= base;
		else
			for( int32_t i=0, count=-exponent; i<count; ++i )
				result /= base;
		return result;
	}

	struct	SPairSinCos								{ double Sin, Cos; };	// This is used to reduce redundancies in constexpr operations.
	
	static inline									SPairSinCos			getSinCos			(double theta)										noexcept	{ return {sin(theta), cos(theta)}; }
	

} // namespace

#endif // __NWOL_MATH_H__92836409283642038462309846__
