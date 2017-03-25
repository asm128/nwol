#include "typeint.h"

#ifndef __SIZE_H__982340928374923__
#define __SIZE_H__982340928374923__

namespace nwol
{
	template <typename _t, size_t _Size>					static inline constexpr uint32_t	size		(const _t (&/*staticArray*/)[_Size])			noexcept	{ return _Size;						}
	template <typename _t, size_t _Depth, size_t _Width>	static inline constexpr uint32_t	size		(const _t (&/*staticArray*/)[_Depth][_Width])	noexcept	{ return _Depth*_Width;				}
	template <typename _t, size_t _Size>					static inline constexpr uint32_t	byte_count	(const _t (&/*staticArray*/)[_Size])			noexcept	{ return sizeof(_t)*_Size;			}
	template <typename _t, size_t _Depth, size_t _Width>	static inline constexpr uint32_t	byte_count	(const _t (&/*staticArray*/)[_Depth][_Width])	noexcept	{ return sizeof(_t)*_Depth*_Width;	}
} // namespace

#endif // __SIZE_H__982340928374923__