#include "label.h"
#include "descriptor.h"
#include "multithread.h"

#ifndef __NWOL_REF_H__9823479023649023642890734__
#define __NWOL_REF_H__9823479023649023642890734__

#define INVALID_ROW (~(uint32_t)0) // 0xFFFFFFFFUL
#define BINFIBO		1458552799L

namespace nwol
{
#pragma pack(push, 1)
#define NWOL_DEBUG_CHECK_TYPE				size_t
#define NWOL_DEBUG_CHECK_VALUE_PRE			BINFIBO
#define NWOL_DEBUG_CHECK_VALUE_POST			BINFIBO
#define NWOL_DEBUG_CHECK_NAME_PRE			DebugCheckPre	
#define NWOL_DEBUG_CHECK_NAME_POST			DebugCheckPost	

#if defined (DEBUG) || defined(_DEBUG)				
#	define __GPRECHECK()						NWOL_DEBUG_CHECK_TYPE	NWOL_DEBUG_CHECK_NAME_PRE	= 	NWOL_DEBUG_CHECK_VALUE_PRE			
#	define __GPOSTCHECK()						NWOL_DEBUG_CHECK_TYPE	NWOL_DEBUG_CHECK_NAME_POST	= 	NWOL_DEBUG_CHECK_VALUE_POST			
#	define __GPRECHECK_ARG()					NWOL_DEBUG_CHECK_TYPE	NWOL_DEBUG_CHECK_NAME_PRE,
#	define __GPOSTCHECK_ARG()					,NWOL_DEBUG_CHECK_TYPE	NWOL_DEBUG_CHECK_NAME_POST	
#	define INVALID_ALLOC_ID						(~(uint64_t)0) //0xFFFFFFFFFFFFFFFFULL
#	define GLOBALSOVERRUNINIT_START				NWOL_DEBUG_CHECK_NAME_PRE(BINFIBO),	// debug check
#	define GLOBALSOVERRUNINIT_END				, NWOL_DEBUG_CHECK_NAME_POST(BINFIBO)	// debug check
#else
#	define __GPRECHECK() 
#	define __GPOSTCHECK()
#	define __GPRECHECK_ARG()
#	define __GPOSTCHECK_ARG()
#	define GLOBALSOVERRUNINIT_START
#	define GLOBALSOVERRUNINIT_END
#endif		

	struct SReferenceGlobals {
																			__GPRECHECK();
									::nwol::cue_t							Cue							= 0;
									void*									__prelease					= nullptr;
									uint32_t								ObjectType					= 0;
									uint32_t								CueLen						= 0;
									uint32_t								DataSize					= 0;
									uint32_t								DataAlign					= 0;
									uint32_t								DataAlignMultiplier			= 0;
									uint32_t								DataSizePadded				= 0;
									uint32_t								Row							= INVALID_ROW;
									uint32_t								PageSizeInInstances			= 0;
																			__GPOSTCHECK();

		constexpr															SReferenceGlobals			()						= default;
		constexpr															SReferenceGlobals
			(	::nwol::cue_t			cue					
			,	uint32_t				objectType			
			,	uint32_t				cueLen				
			,	uint32_t				dataSize			
			,	uint32_t				dataAlign			
			,	uint32_t				dataAlignMultiplier	
			,	uint32_t				dataSizePadded		
			,	uint32_t				row					
			,	uint32_t				pageSizeInInstances	
			,	void*					prelease
			)
		:	GLOBALSOVERRUNINIT_START
			Cue					(cue					)
		,	ObjectType			(objectType				)
		,	CueLen				(cueLen					)
		,	DataSize			(dataSize				)
		,	DataAlign			(dataAlign				)
		,	DataAlignMultiplier	(dataAlignMultiplier	)
		,	DataSizePadded		(dataSizePadded			)
		,	Row					(row					)
		,	PageSizeInInstances	(pageSizeInInstances	)
		,	__prelease			(prelease				)
			GLOBALSOVERRUNINIT_END
		{}

	}; // 

	// This is the common interface for gcore_ref types
	//struct SReferenceInt
	//{
	//};

	template <typename _tBase, ::nwol::GDATA_TYPE... Args> struct gcore_ref //: public SReferenceInt	
	{																
		typedef						gcore_ref<_tBase, Args...>				TRef;	
		typedef						_tBase									TBase;	
		__GPRECHECK();
									::nwol::refcount_t						ReferenceCount;	
									_tBase									* Instance;		
									const ::nwol::SReferenceGlobals			* Globals;		
		__GPOSTCHECK();
#if defined ( DEBUG ) || defined( _DEBUG )				
									uint64_t								AllocID; 
									uint32_t								Column;
		static						uint64_t								__breakAllocID;
#endif	
		inline						TRef*									acquire						()						{ 
			int64_t finalCount;
			if( 1 >= (finalCount = (int64_t)NWOL_INTERLOCKED_INCREMENT(ReferenceCount)) )	{															
				error_printf("Invalid reference count: %llu. Instance type: '%s'.", finalCount, get_type_name().begin());			
				PLATFORM_CRT_BREAKPOINT();											
			}			
			
			return this;	
		}
		inline						_tBase*									get							()						{ return Instance;										}
		inline constexpr			const _tBase*							get							()	const				{ return Instance;										}
		inline constexpr			const bool								shared						()	const				{ return ReferenceCount > 1;							}

		// static members
		static						const ::nwol::cue_t						__kCue;	

		// static accessors
		static inline				const ::nwol::gdescriptor&				get_type_descriptor			()						{
			static constexpr	const ::nwol::GDATA_TYPE							dataTypes[]					= {Args..., (GDATA_TYPE)0};									
			static				const ::nwol::gdescriptor							typeDescriptor				= ::nwol::gdescriptor(dataTypes, ~0U);							
			static				const ::nwol::error_t								err							= ::nwol::validate_type_descriptor<_tBase>(typeDescriptor);	
			return typeDescriptor;																								
		}			

		static inline				const ::nwol::gsyslabel&				get_type_name				()						{
			static	const ::nwol::gsyslabel											typeName					= ::nwol::gsyslabel(__kCue, ~0U);											
			return typeName;																									
		}
	}; // template struct

#pragma pack(pop)
	

	template <typename _tRef>
	static			bool													checkOverrun				(const _tRef* p)		{																										
		if(0 == p) 
			return false;																			
		bool																		r							= false;																						
																											
		// reference check 
		if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_PRE) {																									
			error_printf("Memory was overwritten before the reference metadata. AllocID: %llu. Value: %u. Expected: %u."	, p->AllocID, (uint32_t)p->NWOL_DEBUG_CHECK_NAME_PRE			, (uint32_t)BINFIBO);								
			r	= true;																						
		}																									
		if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_POST) {																									
			error_printf("Memory was overwritten after the reference metadata. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->NWOL_DEBUG_CHECK_NAME_POST			, (uint32_t)BINFIBO);								
			r	= true;																						
		}																									
		// globals check 
		if (BINFIBO != p->Globals->NWOL_DEBUG_CHECK_NAME_PRE) {																									
			error_printf("Memory was overwritten before the globals section. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->Globals->NWOL_DEBUG_CHECK_NAME_PRE	, (uint32_t)BINFIBO);						
			r	= true;																						
		}																									
		if (BINFIBO != p->Globals->NWOL_DEBUG_CHECK_NAME_POST) {																										
			error_printf("Memory was overwritten after the globals section. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->Globals->NWOL_DEBUG_CHECK_NAME_POST	, (uint32_t)BINFIBO);						
			r	= true;																						
		}																									
		if(r) {																									
			error_printf("AllocID: %u.", uint32_t(p->AllocID));												
			//printInfoString(p);																				
		}																									
		return r;																							
	}

	void			nwol_shutdown	();

	template<typename _tRef>	static inline	_tRef*			acquire		(_tRef* instanceRef)								{ return instanceRef ? instanceRef->acquire() : nullptr; }
	template<typename _tRef>	static inline	void			release		(_tRef** instanceRef)								{
		typedef		void			(*TFunctionRelease)	(_tRef**);
		_tRef															* pRef		= *instanceRef;
		*instanceRef												= 0;
		if(pRef)
			((TFunctionRelease)pRef->Globals->__prelease)(&pRef);
	}

	template<typename _tRef>	static inline	void			set			(_tRef** out_Ref, _tRef* in_Ref)					{
		_tRef															* old		= *out_Ref;																
		*out_Ref													= ::nwol::acquire(in_Ref);															
		::nwol::release(&old);																		
	}																																	

	template<typename _tRef>	static inline	void			release		(_tRef** instanceRefs, uint32_t* instanceCount)		{
		for(uint32_t i=0, count = *instanceCount; i<count; ++i)			
			::nwol::release(&instanceRefs[i]);							
	}
} // namespace

#define NWOL_REF(baseType, ...)		::nwol::gcore_ref<baseType, __VA_ARGS__>				

#define GREF(Token)		gref_##Token
#define NWOH(Token)		grefh_##Token
#define GODS(Token)		NWOH(Token)

#define GDECLARE_REF(baseType, ...)		\
	typedef struct NWOL_REF(baseType, __VA_ARGS__)	GREF(baseType), *NWOH(baseType);							

#endif // __NWOL_REF_H__9823479023649023642890734__
