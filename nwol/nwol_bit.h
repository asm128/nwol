#include <cstdint>

#ifndef __NWOL_BIT_H__034792701397__
#define __NWOL_BIT_H__034792701397__

namespace nwol
{
	template<typename _TField>		static inline constexpr	_TField		flag_bit	(uint8_t bitIndex)											{ return (bitIndex >= (sizeof(_TField)*8) ) ?  1 << (bitIndex%(sizeof(_TField)*8)) :  1 << bitIndex; }
	template<typename _TBitField>	inline					_TBitField	bit_set		(_TBitField& inout_FlagVariable, _TBitField in_BitsToSet  )	{ return ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	;}
	template<typename _TBitField>	inline					_TBitField	bit_clear	(_TBitField& inout_FlagVariable, _TBitField in_BitsToClear)	{ return (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					;}
	template<typename _TBitField>	inline constexpr		bool		bit_true	(const _TBitField in_FlagVariable, const _TBitField in_Bit)	{ return ((in_FlagVariable & in_Bit) == in_Bit) ? true : false; }
	template<typename _TBitField>	inline constexpr		bool		bit_false	(const _TBitField in_FlagVariable, const _TBitField in_Bit)	{ return ((in_FlagVariable & in_Bit) != in_Bit) ? true : false; }

#define gflag_bit(bitIndex)								( 1 << bitIndex }
#define gbit_set(inout_FlagVariable, in_BitsToSet  )	( ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	)
#define gbit_clear(inout_FlagVariable, in_BitsToClear)	( (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					)
#define gbit_true(in_FlagVariable, in_Bit)				( ((in_FlagVariable & in_Bit) == in_Bit) ? true : false )
#define gbit_false(in_FlagVariable, in_Bit)				( ((in_FlagVariable & in_Bit) != in_Bit) ? true : false )

}

#endif // __NWOL_BIT_H__034792701397__
