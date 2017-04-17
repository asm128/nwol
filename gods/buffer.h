/// This file is part of the Game Object Data System
/// 2011 (c) Pablo Ariel Zorrilla Cepeda
#include "gref.h"
#include "datausage.h"
#include <cstdio>

#ifndef __BUFFER_H__23984723094__
#define __BUFFER_H__23984723094__

namespace nwol
{
#pragma pack( push, 1 )

	// This structure will be used to communicate memory buffers to the APIs
	struct SBuffer
	{
		uint32_t			nSizeInBytes									;
		uint32_t			nElementCount									;
		uint32_t			nColumnCount									;
		uint32_t			nSliceCount										;
		GDATA_USAGE			Usage											;
		GDATA_TYPE			DataFormat										;
		void*				pByteArray										;
		char*				__pBlock										;
		void				(*pFreeBlock)			(SBuffer*)				;
		void*				(*pAllocBlock)			(uint32_t nSizeInBytes)	;

							~SBuffer				()						{ if(pByteArray) pFreeBlock(this); }
							SBuffer					(const SBuffer& o)		= delete;
		SBuffer&			operator=				(const SBuffer& o)		= delete;
	};
	GDECLARE_NCO( SBuffer );

	void				cloneBuffer					(GREF(SBuffer)** out_pTarget, const GREF(SBuffer)* src_Data);
	void				gcreate					(GREF(SBuffer)** out_newBuffer, const SBuffer & in_cBuffer);
	//__GDECLARE_COMMON_COPIABLE_FUNCTIONS(SBuffer);
	__GDECLARE_POD_SERIALIZATION_FUNCTIONS(SBuffer);	// Buffers support serialization
	int32_t				gcompare				(const GREF(SBuffer)* BufferA, const GREF(SBuffer)* BufferB);
	int32_t				gcompare				(const SBuffer* bufferA, const SBuffer* bufferB);
#pragma pack( pop )

	::nwol::error_t		createBuffer			(GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nMinElementCount, ::nwol::GODS(SBuffer)* out_pBuffer);
	::nwol::error_t		createBuffer			(GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nMinElementCount, uint32_t nColumnCount, ::nwol::GODS(SBuffer)* out_pBuffer);
	::nwol::error_t		createBuffer			(GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nMinElementCount, uint32_t nColumnCount, uint32_t nSliceCount, ::nwol::GODS(SBuffer)* out_pBuffer);
	::nwol::error_t		joinBuffers				(const GODS(SBuffer) in_BufferA, const GODS(SBuffer) in_BufferB, ::nwol::GODS(SBuffer)* out_JoinedBuffer);
	::nwol::error_t		splitBuffer				(const GODS(SBuffer) in_BufferToSplit, uint32_t nIndex, ::nwol::GODS(SBuffer)* out_SplitLeft, ::nwol::GODS(SBuffer)* out_SplitRight);
	

	void*				allocSBufferBlock		(uint32_t sizeInBytes);
	void				freeSBufferBlock		(SBuffer* pBuffer);
	void				checkBlockBoundaries	(SBuffer* pBlock);
};

#endif // __BUFFER_H__23984723094__
