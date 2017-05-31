/// Copyright 2010-2017 - asm128
#include "typeint.h"

#ifndef GCORE_REF_GLOBALS_H_9368498234689723462
#define GCORE_REF_GLOBALS_H_9368498234689723462

namespace nwol 
{
#define INVALID_ROW (~(uint32_t)0) // 0xFFFFFFFFUL
#define BINFIBO		1458552799L

#define NWOL_DEBUG_CHECK_TYPE				size_t
#define NWOL_DEBUG_CHECK_VALUE_PRE			BINFIBO
#define NWOL_DEBUG_CHECK_VALUE_POST			BINFIBO
#define NWOL_DEBUG_CHECK_NAME_PRE			DebugCheckPre	
#define NWOL_DEBUG_CHECK_NAME_POST			DebugCheckPost	

#if defined (NWOL_DEBUG_ENABLED)				
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

	enum GREF_CATEGORY : uint32_t
		{	GREF_CATEGORY_NCO = 0
		,	GREF_CATEGORY_OBJ = 1
		,	GREF_CATEGORY_POD = 2
		};

	struct SReferenceGlobals {
																			__GPRECHECK();
									::nwol::cue_t							Cue							= 0;
									void*									_prelease					= nullptr;
									void*									ReferenceManager			= nullptr;
									GREF_CATEGORY							ObjectCategory				= GREF_CATEGORY_NCO;
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
			,	GREF_CATEGORY			objectType			
			,	uint32_t				cueLen				
			,	uint32_t				dataSize			
			,	uint32_t				dataAlign			
			,	uint32_t				dataAlignMultiplier	
			,	uint32_t				dataSizePadded		
			,	uint32_t				row					
			,	uint32_t				pageSizeInInstances	
			,	void*					prelease
			,	void*					referenceManager
			)
		:	GLOBALSOVERRUNINIT_START
			Cue					(cue					)
		,	ObjectCategory		(objectType				)
		,	CueLen				(cueLen					)
		,	DataSize			(dataSize				)
		,	DataAlign			(dataAlign				)
		,	DataAlignMultiplier	(dataAlignMultiplier	)
		,	DataSizePadded		(dataSizePadded			)
		,	Row					(row					)
		,	PageSizeInInstances	(pageSizeInInstances	)
		,	_prelease			(prelease				)
		,	ReferenceManager	(referenceManager		)
			GLOBALSOVERRUNINIT_END
		{}

	}; // 
} // namespace

#endif // GCORE_REF_GLOBALS_H_9368498234689723462
