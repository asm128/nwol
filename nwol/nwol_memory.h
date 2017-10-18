/// Copyright 2010-2017 - asm128
#include "nwol_evaluation.h"
#include <malloc.h>
#include "nwol_safe.h"
#include "nwol_platform_handle_wrapper.h"

#ifndef NWOL_MEMORY_H__92836409283642038462309846
#define NWOL_MEMORY_H__92836409283642038462309846

#define GREF_CUSTOM_ALIGN					16
#define NWOL_MALLOC_CUSTOM_ALIGN			16

#ifdef GREF_CUSTOM_ALIGN
#define BASETYPE_ALIGN						GREF_CUSTOM_ALIGN
#else
#define BASETYPE_ALIGN						sizeof(void*)
#endif

#define calc_align_address(alignment, address)	((alignment - ((alignment - 1) & (uintptr_t)address)) & (alignment - 1))	// returns the difference between an origin address and the next aligned addres. The alignment must always be a power of two.
#define calc_align_address_4( address)		calc_align_address( 4, address)	// (0x04 - (0x03 & (uintptr_t)address) & 0x03)
#define calc_align_address_8( address)		calc_align_address( 8, address)	// (0x08 - (0x07 & (uintptr_t)address) & 0x07)
#define calc_align_address_16(address)		calc_align_address(16, address)	// (0x10 - (0x0F & (uintptr_t)address) & 0x0F)
#define calc_align_address_32(address)		calc_align_address(32, address)	// (0x20 - (0x1F & (uintptr_t)address) & 0x1F)
#define calc_align_address_64(address)		calc_align_address(64, address)	// (0x40 - (0x3F & (uintptr_t)address) & 0x3F)

namespace nwol
{
#if defined(__WINDOWS__)
	static inline void*																			nwol_malloc					(size_t size)													{ return _aligned_malloc(size, NWOL_MALLOC_CUSTOM_ALIGN);	}
	static inline void																			nwol_free					(void* ptr)														{ _aligned_free(ptr);										}
#elif defined(__LINUX__) || defined(__ANDROID__)
	static inline void*																			nwol_malloc					(size_t size)													{ return ::memalign(NWOL_MALLOC_CUSTOM_ALIGN, size);		}
	static inline void																			nwol_free					(void* ptr)														{ ::free(ptr);												}
#endif

	template<typename _typePtr>
	static inline void																			safe_nwol_free				(_typePtr &p)													{ 
		_typePtr																						_pepe						= p; 
		p																							= 0; 
		_aligned_free(_pepe);
	}

	struct auto_nwol_free : public ::nwol::platform_handle_wrapper<void*, 0>					{ using TWrapper::platform_handle_wrapper; inline ~auto_nwol_free() { close(); } inline void close() { safe_nwol_free(Handle); } };

#define GREF_PAGE_SIZE_MAX (4096)
	template<typename _tBase>	static inline constexpr		uint32_t							get_page_size				()													noexcept	{ return (uint32_t)(sizeof(_tBase) <= GREF_PAGE_SIZE_MAX) ? GREF_PAGE_SIZE_MAX/sizeof(_tBase) : 1; };
	template<typename _tBase>	static inline constexpr		uint32_t							get_type_size				()													noexcept	{ return (uint32_t) sizeof(_tBase); }
	template<typename _tBase>	static inline constexpr		uint32_t							get_type_size				(const _tBase&)										noexcept	{ return (uint32_t) sizeof(_tBase); }
	template<typename _tBase>	static inline constexpr		uint32_t							get_type_size_padded		(uint32_t paddingInBytes)										{ 
		return (uint32_t) ( (sizeof(_tBase) / paddingInBytes) + one_if(sizeof(_tBase) % paddingInBytes) ) * paddingInBytes; 
	}
	template <typename _tBase>	static inline constexpr		uint32_t							get_type_align				()													noexcept	{
		return (uint32_t)	
			(	(0 == (sizeof(_tBase) % 32))	? 32	
			:	(0 == (sizeof(_tBase) % 16))	? 16	
			:	(0 == (sizeof(_tBase) % 8 ))	? 8		
			:	(0 == (sizeof(_tBase) % 4 ))	? 4		
			:	(0 == (sizeof(_tBase) % 2 ))	? 2		
			:	1
			);
	}
	template <typename _tBase>	static inline constexpr		uint32_t							get_type_align_multiplier	()													noexcept	{
		return (uint32_t) 
			(	(0 == (sizeof(_tBase) % 32)) ? sizeof(_tBase) / 32
			:	(0 == (sizeof(_tBase) % 16)) ? sizeof(_tBase) / 16
			:	(0 == (sizeof(_tBase) %  8)) ? sizeof(_tBase) /  8
			:	(0 == (sizeof(_tBase) %  4)) ? sizeof(_tBase) /  4
			:	(0 == (sizeof(_tBase) %  2)) ? sizeof(_tBase) /  2
			:	sizeof(_tBase) 
			);
	}
	template <typename _tBase>	static inline				int32_t								podcmp						(const _tBase* pA, const _tBase* pB)							{
		if (0 == pA && 0 == pB)		
			return 0;				
		else if (0 == pA || 0 == pB)
			return 0x7FFFFFFF;		
									
		return memcmp(pA, pB, sizeof(_tBase));							
	}
	template <typename _tBase>	static inline				_tBase*								chkcpy						(_tBase* destination, const _tBase* source, uint32_t count)		{
			for (uint32_t i = 0; i < count; ++i)
				if (destination[i] != source[i])
					destination[i]  = source[i];
			return destination;
	}
	template <typename _tBase>								_tBase*								podcpy						(_tBase* dest, const _tBase* source)							{
		static constexpr	const uint32_t																dataMultiplier				= (uint32_t)get_type_align_multiplier<_tBase>();
		static constexpr	const uint32_t																typeAlign					= (uint32_t)get_type_align<_tBase>();
		switch (typeAlign) {
		case 32: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier * 4);	break;
		case 16: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier * 2);	break;
		case  8: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier);		break;
		case  4: chkcpy((uint32_t*)dest, (const uint32_t*)source, dataMultiplier);		break;
		case  2: chkcpy((uint16_t*)dest, (const uint16_t*)source, dataMultiplier);		break;
		default:
			if( memcmp(dest, source, sizeof(_tBase)) )
				memcpy(dest, source, sizeof(_tBase));
			break;
		}
		return dest;
	}

	template <typename _tBase, size_t _sizeArray>			_tBase*								memcpy_s					(_tBase (&dest)[_sizeArray], const _tBase (&src)[_sizeArray])	{
		memcpy_s(dest, sizeof(_tBase) * _sizeArray, src, sizeof(_tBase) * _sizeArray);
	}

}	// namespace

#endif // NWOL_MEMORY_H__92836409283642038462309846
