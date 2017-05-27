#include <cstdint>

#ifndef __NWOL_BIT_H__034792701397__
#define __NWOL_BIT_H__034792701397__

namespace nwol
{
	template<typename _tField>		static inline constexpr	_tField		flag_bit	(uint8_t bitIndex)											noexcept	{ return (bitIndex >= (sizeof(_tField)*8) ) ?  1 << (bitIndex%(sizeof(_tField)*8)) :  1 << bitIndex; }
	template<typename _tBitField>	inline					_tBitField	bit_set		(_tBitField& inout_FlagVariable, _tBitField in_BitsToSet  )	noexcept	{ return ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	;}
	template<typename _tBitField>	inline					_tBitField	bit_clear	(_tBitField& inout_FlagVariable, _tBitField in_BitsToClear)	noexcept	{ return (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					;}
	template<typename _tBitField>	inline constexpr		bool		bit_true	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) == in_Bit) ? true : false; }
	template<typename _tBitField>	inline constexpr		bool		bit_false	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) != in_Bit) ? true : false; }

#define gflag_bit(bitIndex)								( 1 << (bitIndex) )
#define gbit_set(inout_FlagVariable, in_BitsToSet  )	( (((inout_FlagVariable) & (in_BitsToSet)) == (in_BitsToSet))	? (inout_FlagVariable)						: (inout_FlagVariable) |= (in_BitsToSet)	)
#define gbit_clear(inout_FlagVariable, in_BitsToClear)	( ((inout_FlagVariable) & (in_BitsToClear))						? (inout_FlagVariable) &= ~(in_BitsToClear)	: (inout_FlagVariable)						)
#define gbit_true(in_FlagVariable, in_Bit)				( (((in_FlagVariable) & (in_Bit)) == (in_Bit)) ? true : false )
#define gbit_false(in_FlagVariable, in_Bit)				( (((in_FlagVariable) & (in_Bit)) != (in_Bit)) ? true : false )
}

#endif // __NWOL_BIT_H__034792701397__
