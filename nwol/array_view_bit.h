#include "nwol_log.h"

#ifndef __ARRAY_VIEW_BIT_H__892364987234__
#define __ARRAY_VIEW_BIT_H__892364987234__

namespace nwol 
{
#pragma pack(push, 1)
	template<typename _tValue>	
	class array_view_bit {
							_tValue*				Data			= 0;
							uint32_t				Count			= 0;
		static constexpr	const uint32_t			SizeBase		= (uint32_t)(sizeof(_tValue)*8);
	public:
		inline constexpr							array_view_bit	()														noexcept	= default;
		inline constexpr							array_view_bit	(array_view_bit	<_tValue>& other)						noexcept	= default;
		inline constexpr							array_view_bit	(_tValue* data, int32_t count)							noexcept	: Data(data), Count(count)					{}

							bool					operator[]		(uint32_t index)								const				{
			if(index >= 8*sizeof(_tValue)*Count) {
				error_printf("Invalid index: %u.", index);
			}
			return Data[index/SizeBase] & (1 << (index%SizeBase));
		}
	};

	template<typename _tValue>	
	class grid_view_bit	{
							_tValue*				Data			= 0;
	public:
							uint32_t				Width			= 0;
							uint32_t				Depth			= 0;
		inline constexpr							grid_view_bit	()														noexcept	= default;
		inline constexpr							grid_view_bit	(const grid_view_bit<_tValue>& other)					noexcept	= default;
		inline constexpr							grid_view_bit	(_tValue* data, int32_t width, int32_t height)			noexcept	: Data(data), Width(width), Depth(height)	{}

							array_view_bit<_tValue>	operator[]		(uint32_t index)								const				{
			if(index >= Depth) {
				error_printf("Invalid index: %u.", index);
			}
			return array_view_bit<_tValue>(&Data[index*(Width/SizeBase)], Width);
		}
	};
#pragma pack(pop)
}

#endif