
#include "buffer.h"

#include "gref_definition.h"
#include "nwol_string.h"

#include "glist.h"

__GCORE_REF_INIT_STATIC_MEMBERS(nwol, SBuffer)			
namespace nwol {									
			void															grelease				(GODS(SBuffer)*);
	static	::nwol::gref_manager_nco<GREF(SBuffer), GREF_CATEGORY_NCO>	__g_SBufferManager	(grelease);
	//__GDEFINE_ACQUIRE(SBuffer);				
	__GDEFINE_ALLOC(SBuffer);																			
}	

//
void																		nwol::gcreate			( GREF(SBuffer)** out_newBuffer, const SBuffer & in_cBuffer )	{
	GODS(SBuffer)																		pnewBuffer			= 0;
	::nwol::createBuffer( in_cBuffer.DataFormat, in_cBuffer.Usage, in_cBuffer.nElementCount, in_cBuffer.nColumnCount, in_cBuffer.nSliceCount, &pnewBuffer );
	if(in_cBuffer.pByteArray && pnewBuffer && pnewBuffer->get()->pByteArray)
		memcpy(pnewBuffer->get()->pByteArray, in_cBuffer.pByteArray,  nwol::min(pnewBuffer->get()->nSizeInBytes, in_cBuffer.nSizeInBytes));
	GODS(SBuffer)																		old					= * out_newBuffer;
	* out_newBuffer																	= pnewBuffer;
	::nwol::release(&old);
}

namespace nwol
{
#pragma pack( push, 1 )
	struct SDataTypeInfo {
		uint32_t		ElementBytes	;
		uint32_t		TotalBytes		;

		uint8_t			IsBigEndian		:1;
		uint8_t			IsNorm			:1;
		uint8_t			IsFloat			:1;
		uint8_t			IsSigned		:1;
	};
#pragma pack(pop)

	struct SBufferManagerStorage : public nwol::array_pod<GODS(SBuffer)>, public CMutex	{																														
										~SBufferManagerStorage		()										{ clear(); }

		int32_t							clear						()										{
			if( Data ) {
				uint32_t nTotalInstances=0;
				for( uint32_t iBuffer=0, nCount = size(); iBuffer<nCount; iBuffer++ ) 
					if( Data[iBuffer] ) {
						Data[iBuffer]->ReferenceCount++;
						__g_SBufferManager.releaseRef(&Data[iBuffer]);
						nTotalInstances ++;
					}
				info_printf("SBuffer instances released:%u", nTotalInstances);
			}
			return array_pod<GODS(SBuffer)>::clear();
		}
	};																																	


	class __CBufferManager {
	public:
										__CBufferManager			()										{ (void)INIT_CRITICAL_SECTION(cs); }
										~__CBufferManager			()										{
			info_printf("Buffer manager shutting down.");

			DELETE_CRITICAL_SECTION(cs);

			for( uint32_t iBigEndian	= 0; iBigEndian		< 2		; ++iBigEndian		)
			for( uint32_t iNorm			= 0; iNorm			< 2		; ++iNorm			)
			for( uint32_t iFloat		= 0; iFloat			< 2		; ++iFloat			)
			for( uint32_t iSigned		= 0; iSigned		< 2		; ++iSigned			)
			for( uint32_t iElementBytes	= 0; iElementBytes	< 16	; ++iElementBytes	)
			for( uint32_t iTotalBytes	= 0; iTotalBytes	< 1024	; ++iTotalBytes		) {
				SBufferManagerStorage				* pStorage = Storage[iBigEndian][iNorm][iFloat][iSigned][iElementBytes][iTotalBytes];
				if( pStorage ) {
					info_printf("Deleting SBuffer* storage:\n"
						"isBigEndian  :%s\n"
						"isNorm       :%s\n"
						"isFloat      :%s\n"
						"isSigned     :%s\n"
						"ElementBytes :%u\n"
						"TotalBytes   :%u\n"
						, iBigEndian	? "true" : "false"
						, iNorm			? "true" : "false"
						, iFloat		? "true" : "false"
						, iSigned		? "true" : "false"
						, iElementBytes
						, iTotalBytes
						);
					delete(pStorage);
				}
			}
		}

		void							releaseBuffer				(GODS(SBuffer)* pBuffer)				{
			GODS(SBuffer)						oldBuffer					= *pBuffer;
			*pBuffer						= 0;
			CHECKBUFFEROVERRUNREF(oldBuffer);
			SDataTypeInfo						typeInfo;
			getTypeInfoFromTypeID(oldBuffer->get()->DataFormat, typeInfo);
			ENTER_CRITICAL_SECTION(cs);
			SBufferManagerStorage				* pStorage					= getStorage(typeInfo);
			if( 0 == pStorage ) {
				pStorage						= new SBufferManagerStorage();
				if( 0 == pStorage ) {
					error_printf("Cannot create buffer pool. Out of memory?");
					oldBuffer->ReferenceCount++;	// We need to fix this value or the base SBuffer manager will detect an error.
					__g_SBufferManager.releaseRef(&oldBuffer);
				}
				else
					releaseToPage(pStorage, oldBuffer);

				setStorage(pStorage, typeInfo);
				LEAVE_CRITICAL_SECTION(cs);
			}
			else {
				LEAVE_CRITICAL_SECTION(cs);
				releaseToPageLocked(pStorage, oldBuffer);
			}
			nwol_sync_increment(Counters.FreedRefs);
		}
	
		::nwol::error_t					createBuffer				(GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nElementCount, uint32_t nColumnCount, uint32_t nSliceCount, ::nwol::GODS(SBuffer)* out_pBuffer)	{
			::nwol::error_t						errMy						= 0;
			SDataTypeInfo						typeInfo;
			getTypeInfoFromTypeID(DataFormat, typeInfo);
			GODS(SBuffer)						newBuffer					= 0;
			ENTER_CRITICAL_SECTION(cs);
			SBufferManagerStorage				* pStorage					= getStorage(typeInfo);
			if( 0 == pStorage ) {
				::nwol::galloc( &newBuffer );
				if( 0 == newBuffer ) {
					error_printf("Cannot create new buffer. Out of memory?");
					errMy							= -1;
				}
				else if( initBuffer(newBuffer->get(), (uint32_t)(nElementCount+nElementCount*.25f)*typeInfo.TotalBytes, DataFormat, Usage, nElementCount, nColumnCount, nSliceCount ) ) {
					error_printf("Failed to initialize SBuffer instance.");
					errMy							= -1;
				};
				LEAVE_CRITICAL_SECTION(cs);
				CHECKBUFFEROVERRUNREF(newBuffer);
			}
			else {
				LEAVE_CRITICAL_SECTION(cs);
				uint32_t							sizeRequested				= typeInfo.TotalBytes*nElementCount;
				pStorage->lock();
				newBuffer						= getBufferFromStorage(pStorage, sizeRequested); // (uint32_t)(sizeRequested+sizeRequested*.25f));
				pStorage->unlock();
				if( newBuffer ) {
					SBuffer								* pBuffer = newBuffer->get();
					pBuffer->DataFormat				= DataFormat;
					pBuffer->nElementCount			= nElementCount;
					pBuffer->nSliceCount			= nSliceCount;
					pBuffer->nColumnCount			= nColumnCount;
					pBuffer->Usage					= Usage;
				}
				else {
					::nwol::galloc( &newBuffer );
					if( 0 == newBuffer ) {
						error_printf("Cannot create new buffer. Out of memory?");
						errMy							= -1;
					}
					else if( initBuffer(newBuffer->get(), (uint32_t)(nElementCount+nElementCount*.25f)*typeInfo.TotalBytes, DataFormat, Usage, nElementCount, nColumnCount, nSliceCount ) ) {
						error_printf("Failed to initialize SBuffer instance.");
						errMy							= -1;
					};
				}
			}
			if( newBuffer )
				nwol_sync_increment(Counters.CreatedRefs);

			CHECKBUFFEROVERRUNREF(newBuffer);
			GODS(SBuffer)						old							= *out_pBuffer;
			CHECKBUFFEROVERRUNREF(old);
			*out_pBuffer					= newBuffer;
			::nwol::release(&old);
			return errMy;
		}

		void							shutdown					() {
			for( uint32_t iBigEndian	= 0; iBigEndian		< 2		; ++iBigEndian		)
			for( uint32_t iNorm			= 0; iNorm			< 2		; ++iNorm			)
			for( uint32_t iFloat		= 0; iFloat			< 2		; ++iFloat			)
			for( uint32_t iSigned		= 0; iSigned		< 2		; ++iSigned			)
			for( uint32_t iElementBytes	= 0; iElementBytes	< 16	; ++iElementBytes	)
			for( uint32_t iTotalBytes	= 0; iTotalBytes	< 1024	; ++iTotalBytes		) {
				SBufferManagerStorage				* pStorage					= Storage[iBigEndian][iNorm][iFloat][iSigned][iElementBytes][iTotalBytes];
				if( pStorage )
					pStorage->clear();
			}
		}
	protected:
		// IsBigEndian, IsNorm, IsFloat, IsSigned, SizeInBits/8,
		SBufferManagerStorage													* Storage[2][2][2][2][16][1024]			= {};	// up to 128 bit of element size

		struct SCounters {					
#if defined(__WINDOWS__)
			refcount_t															CreatedRefs								= 0;		
			refcount_t															AcquiredRefs							= 0;		
			refcount_t															FreedRefs								= 0;			
#else
			refcount_t															CreatedRefs								;		
			refcount_t															AcquiredRefs							;		
			refcount_t															FreedRefs								;			
#endif
		};	
		DECLARE_CRITICAL_SECTION(cs);
		SCounters						Counters;

		::nwol::error_t					initBuffer				(SBuffer* pBuffer, uint32_t nSizeInBytes, GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nElementCount, uint32_t nColumnCount, uint32_t nSliceCount) {
			//new (pBuffer) SBuffer();
			pBuffer->DataFormat				= DataFormat;
			pBuffer->Usage					= Usage;
			pBuffer->nElementCount			= nElementCount;
			pBuffer->nColumnCount			= nColumnCount;
			pBuffer->nSliceCount			= nSliceCount;
			pBuffer->nSizeInBytes			= nSizeInBytes;
			pBuffer->pFreeBlock				= freeSBufferBlock;
			pBuffer->pAllocBlock			= allocSBufferBlock;
			pBuffer->__pBlock				= (char*)pBuffer->pAllocBlock(pBuffer->nSizeInBytes);
			if(0 == pBuffer->__pBlock) {
				error_printf("Cannot allocate buffer block! Out of memory? Size requested %u", pBuffer->nSizeInBytes);
				return -1;
			}
			else
				pBuffer->pByteArray				= &pBuffer->__pBlock[calc_align_address_16(pBuffer->__pBlock)];

			return 0;
		}

		// Retrieve an already existing buffer instance from the pool. 
		GODS(SBuffer)					getBufferFromStorage	(SBufferManagerStorage* pStorage, uint32_t sizeRequested)					{
			GODS(SBuffer)						newBuffer				= 0;
			uint32_t							maxSize					= (uint32_t)(sizeRequested+sizeRequested*.25f);
			for( uint32_t iBuffer=0, nCount=pStorage->size(); iBuffer<nCount; iBuffer++) {
				GODS(SBuffer)						pUnusedInstance			= (*pStorage)[iBuffer];
				uint32_t							nSizeInBytes			= pUnusedInstance->get()->nSizeInBytes;
				if( nSizeInBytes >= sizeRequested && nSizeInBytes <= maxSize ) {
					pUnusedInstance->ReferenceCount++;
					(*pStorage)[iBuffer]			= (*pStorage)[nCount-1];
					pStorage->resize(nCount-1);
					newBuffer						= pUnusedInstance;
					break;
				}
			}
			return newBuffer;
		}

		::nwol::error_t					releaseToPage			(SBufferManagerStorage* pStorage, GODS(SBuffer) oldBuffer)					{
			int32_t								expectedIndex			= pStorage->size();
			int32_t								newIndex				= pStorage->push_back(oldBuffer);
			if( expectedIndex != newIndex ) {
				warning_printf("No more space for buffer caching!");
				++oldBuffer->ReferenceCount;
				__g_SBufferManager.releaseRef(&oldBuffer);	// no more space, return to the main buffer manager.
			}
			return 0;
		}

		inline ::nwol::error_t			releaseToPageLocked		(SBufferManagerStorage* pStorage, GODS(SBuffer) oldBuffer)					{
			pStorage->lock();
			releaseToPage(pStorage, oldBuffer);
			pStorage->unlock();
			return 0;
		}
		
		// Retrieve an already existing storage instance or NULL instead. 
		inline SBufferManagerStorage	* getStorage			(const SDataTypeInfo& typeInfo)												{
			return Storage[typeInfo.IsBigEndian][typeInfo.IsNorm][typeInfo.IsFloat][typeInfo.IsSigned][typeInfo.ElementBytes][typeInfo.TotalBytes%1024];
		}

		inline SBufferManagerStorage	* setStorage			(SBufferManagerStorage* pStorage, const SDataTypeInfo& typeInfo)			{
			Storage[typeInfo.IsBigEndian][typeInfo.IsNorm][typeInfo.IsFloat][typeInfo.IsSigned][typeInfo.ElementBytes][typeInfo.TotalBytes%1024] = pStorage;
			return pStorage;
		}

		static inline error_t			getTypeInfoFromTypeID	(GDATA_TYPE dt, SDataTypeInfo& typeInfo)									{
			typeInfo.IsBigEndian			= one_if(GTYPEID_ISBIGENDIAN	(dt));
			typeInfo.IsNorm					= one_if(GTYPEID_ISNORMALIZED	(dt));
			typeInfo.IsFloat				= one_if(GTYPEID_ISFLOAT		(dt));
			typeInfo.IsSigned				= one_if(GTYPEID_ISSIGNED		(dt));
			typeInfo.ElementBytes			= GTYPEID_ELEMENTBYTES			(dt);
			typeInfo.TotalBytes				= GTYPEID_TOTALBYTES			(dt);

			return 0;
		}

	};

	static	nwol::__CBufferManager	__g_BufferManager;
}	

void	nwol::nwol_shutdown		()							{ ::nwol::__g_BufferManager.shutdown(); }


void*						nwol::allocSBufferBlock		(uint32_t sizeInBytes)		{
#if defined(NWOL_DEBUG_ENABLED)
	char* pBlock = (char*)malloc(sizeInBytes+sizeof(NWOL_DEBUG_CHECK_TYPE)*2+BASETYPE_ALIGN);

	// Handle errors outside this function.
	if( 0 == pBlock )	
		return 0;

	// assign check before buffer
	NWOL_DEBUG_CHECK_TYPE* pCheckPre		= (NWOL_DEBUG_CHECK_TYPE*)pBlock;
	pCheckPre[0]	= BINFIBO;

	// fix address
	pBlock = &pBlock[sizeof(NWOL_DEBUG_CHECK_TYPE)];

	// assign check at end of buffer
	NWOL_DEBUG_CHECK_TYPE* pPostCheck	= (NWOL_DEBUG_CHECK_TYPE*)&pBlock[sizeInBytes+calc_align_address(BASETYPE_ALIGN, pBlock)];
	pPostCheck[0]	= BINFIBO;
	return pBlock;
#else
	return malloc(sizeInBytes+BASETYPE_ALIGN);
#endif
}

void						nwol::freeSBufferBlock		(SBuffer* pBuffer)			{
#if defined(NWOL_DEBUG_ENABLED)
	checkBlockBoundaries(pBuffer);
	free(pBuffer->__pBlock - sizeof(NWOL_DEBUG_CHECK_TYPE));
#else
	free(pBuffer->__pBlock);
#endif
}

void						nwol::checkBlockBoundaries	(SBuffer* pBuffer)			{
//#if defined(NWOL_DEBUG_ENABLED)
	char							* pBlock					= pBuffer->__pBlock;
	NWOL_DEBUG_CHECK_TYPE			checkPre					= *(NWOL_DEBUG_CHECK_TYPE*)	(pBlock-sizeof(NWOL_DEBUG_CHECK_TYPE));
	NWOL_DEBUG_CHECK_TYPE			checkPost					= *(NWOL_DEBUG_CHECK_TYPE*)	&pBlock[pBuffer->nSizeInBytes+calc_align_address(BASETYPE_ALIGN, pBlock)];
	if( checkPre != BINFIBO ) {
		error_printf("Memory written before buffer writable block:<SBuffer*>=0x%p. Block address:0x%p. Check value: 0x%X. Current value: 0x%X.", pBuffer, pBlock, BINFIBO, checkPre);
		_CrtDbgBreak();
	}
	if( checkPost != BINFIBO ) {
		error_printf("Memory written after buffer writable block:<SBuffer*>=0x%p. Block address:0x%p. Check value: 0x%X. Current value: 0x%X.", pBuffer, pBlock, BINFIBO , checkPost);
		_CrtDbgBreak();
	}
//#endif
}

nwol::error_t				nwol::createBuffer			(GDATA_TYPE DataFormat, GDATA_USAGE Usage, uint32_t nElementCount, uint32_t nColumnCount, uint32_t nSliceCount, ::nwol::GODS(SBuffer)* out_pBuffer)		{
	if( Usage == GUSAGE_TEXT )
		nElementCount					+= 1;

	::nwol::error_t						errMy					= 0;
	::nwol::GODS(SBuffer)				newBuffer				= 0;

	if( 0 != (errMy = __g_BufferManager.createBuffer(DataFormat, Usage, nElementCount, nColumnCount, nSliceCount, &newBuffer)) ) {
		error_printf("Cannot create new SBuffer instance.");
		errMy							= -1;
	}
	else if( newBuffer && (GUSAGE_TEXT == (newBuffer->get()->Usage = Usage)) ) {
		uint32_t							typeSize				= GTYPEID_TOTALBYTES(DataFormat);
		SBuffer								* buf					= newBuffer->get();
		buf->nElementCount				-= 1;
		memset( &((char*)buf->pByteArray)[buf->nElementCount*typeSize], 0, typeSize );
	}
	GPNCO(::nwol, SBuffer)				oldBuffer				= *out_pBuffer;
	*out_pBuffer					= newBuffer;
	return errMy;
}

void nwol::grelease( GODS(SBuffer)* pBufferData ) {
	if( 0 == (*pBufferData) ) 
		return;

	::nwol::GODS(SBuffer) oldBuffer = *pBufferData;
	*pBufferData = 0;
#if defined(NWOL_DEBUG_ENABLED)
	SBuffer* pBufferInstance = oldBuffer->get();
	if( pBufferInstance->pByteArray )
		checkBlockBoundaries(pBufferInstance);
#endif	
	switch(nwol_sync_decrement(oldBuffer->ReferenceCount)) {
	case ((REFCOUNT_T)0)-1:
		errmsg_refcountnull();
		//printInfoString( oldBuffer );
		PLATFORM_CRT_BREAKPOINT();
		oldBuffer->ReferenceCount = 0;
	case 0:
		__g_BufferManager.releaseBuffer(&oldBuffer);
	default:
		break;
	}
}
