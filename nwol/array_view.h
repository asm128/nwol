/// Copyright 2016-2017 - asm128
#include "nwol_log.h"
#include "typeint.h"

#ifndef __ARRAY_VIEW_H__926340923746098743230948__
#define __ARRAY_VIEW_H__926340923746098743230948__

namespace nwol
{
	template<typename _tBase>
	class array_view {
	protected:
							_tBase						* Data					= nullptr;
							uint32_t					Count					= 0;
	public:
		typedef				_tBase						TBase					;
		static				const uint32_t				ELEMENT_SIZE			= (uint32_t)sizeof(_tBase);

		template<size_t _arraySize>
		inline constexpr								array_view				(_tBase (&data)[_arraySize])			noexcept	: Data(data), Count((uint32_t)_arraySize)	{}
		inline constexpr								array_view				()										noexcept	= default;
		inline											array_view				(_tBase* data, uint32_t count)						: Data(data), Count(data ? count : 0U)	{ 
			throw_if((nullptr == data) && count, "", "array_view initialized with a null pointer and a counter with value %u", count); 
		}
		inline				const _tBase&				operator []				(uint32_t index)				const				{ throw_if(0 == Data, "", "%s", "Uninitialized array pointer!"); error_if(index >= Count, "%s: %u", "Invalid array index", index); return Data[index]; }
		inline				_tBase&						operator []				(uint32_t index)									{ throw_if(0 == Data, "", "%s", "Uninitialized array pointer!"); error_if(index >= Count, "%s: %u", "Invalid array index", index); return Data[index]; }

		inline constexpr	operator					bool					()								const	noexcept	{ return Count > 0 ? true : false;		}

		inline				_tBase*						begin					()										noexcept	{ return Data;							}
		inline				_tBase*						end						()										noexcept	{ return Data + Count;					}

		inline constexpr	const _tBase*				begin					()								const	noexcept	{ return Data;							}
		inline constexpr	const _tBase*				end						()								const	noexcept	{ return Data + Count;					}

		inline constexpr	bool						empty					()								const	noexcept	{ return 0 == Count;					}
		inline constexpr	uint32_t					size					()								const	noexcept	{ return Count;							}
	};
}

#endif // __ARRAY_VIEW_H__926340923746098743230948__
