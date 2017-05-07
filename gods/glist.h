#include "glist_base.h"
#include "gref_serializer_pod.h"

#ifndef __GLIST_H__93873294__
#define __GLIST_H__93873294__

namespace nwol
{
#define GListNCO(baseType)				::nwol::glist_nco			<GREF(baseType)>
#define GLstNCO(NameSpace, baseType)	::nwol::glist_nco			<NameSpace::GREF(baseType)>
	template<typename _tRef, ::nwol::GDATA_TYPE m_DataType = GTYPEID_POINTER_MAKE(_tRef), int32_t m_DataBytes = GTYPEID_TOTALBYTES(GTYPEID_POINTER_MAKE(_tRef))> class glist_nco : public glist_base<_tRef, m_DataType, m_DataBytes>
	{
		typedef glist_base<_tRef, m_DataType, m_DataBytes>	base_list_type;
		using			base_list_type::					glist_base;
	public:
															glist_nco				()																															= default;
															glist_nco				(uint32_t nInitialSize)																										{	resize(nInitialSize);	}
		//
		virtual			::nwol::error_t						create					(_tRef** in_lstCoreInstances, uint32_t nInstanceCount)																		{
			reterr_error_if(nInstanceCount && 0 == in_lstCoreInstances, "%s", "Cannot create list from a null address!");

			if( this->m_BufferData.writable()
				&& (this->Count >= nInstanceCount || this->m_BufferData->nSizeInBytes >= (nInstanceCount*m_DataBytes) ) )
			{
				uint32_t												iCoreInstance;	
				_tRef**													pRefArray				= (_tRef**)this->m_BufferData->pByteArray;
				::nwol::release(pRefArray, &this->Count);

				for(iCoreInstance = 0; iCoreInstance < nInstanceCount; iCoreInstance++)
					pRefArray[iCoreInstance]							= ::nwol::acquire(in_lstCoreInstances[iCoreInstance]);

				this->m_BufferData->nElementCount = this->Count = nInstanceCount;
			}
			else {
				GPNCO(::nwol, SBuffer)									newListBuffer;
				reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nInstanceCount, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", nInstanceCount);

				for (uint32_t iCoreInstance = 0; iCoreInstance < nInstanceCount; iCoreInstance++)
					((_tRef**)newListBuffer->pByteArray)[iCoreInstance]	= ::nwol::acquire(in_lstCoreInstances[iCoreInstance]);

				this->SetBufferData(newListBuffer);
			}
			return 0;
		}

		//
		virtual			::nwol::error_t						create					(::nwol::gptr_nco<_tRef>* in_lstCoreInstances, uint32_t nInstanceCount)														{
			if (nInstanceCount && 0 == in_lstCoreInstances) {
				error_printf("%s", "Cannot create list from a null address!");
				return -1;
			}
			GPNCO(::nwol, SBuffer)										newListBuffer;
			reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nInstanceCount, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", nInstanceCount);
			for (uint32_t iCoreInstance = 0; iCoreInstance < nInstanceCount; iCoreInstance++)
				((_tRef**)newListBuffer->pByteArray)[iCoreInstance]		= in_lstCoreInstances[iCoreInstance].acquire();
		
			this->SetBufferData(newListBuffer);

			return 0;
		}

		//
		virtual			::nwol::error_t						resize					(uint32_t newSize)																											{
			if (newSize == this->Count)
				return 0;				// size already set
			else if (0 == newSize) {
				this->SetBufferData(0);	// clear list
				return 0;
			}
			else if (this->m_BufferData && this->m_BufferData.am_I_owning() && newSize <= (this->m_BufferData->nSizeInBytes / m_DataBytes)) {
				if (newSize > this->Count)
					memset(&((_tRef**)this->m_BufferData->pByteArray)[this->Count], 0, sizeof(_tRef*)*(newSize - this->Count));
				else
					for (uint32_t iRef = (uint32_t)newSize; iRef < this->Count; iRef++)
						::nwol::release(&((_tRef**)this->m_BufferData->pByteArray)[iRef]);

				this->m_BufferData->nElementCount = this->Count			= (uint32_t)newSize;
			}
			else {
				GPNCO(::nwol, SBuffer)								newListBuffer;
				reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, (uint32_t)newSize, this->m_BufferData ? this->m_BufferData->nColumnCount : (uint32_t)newSize, this->m_BufferData ? this->m_BufferData->nSliceCount : 1, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", newSize);
				if (this->Count) {
					GPNCO(::nwol, SBuffer)								oldListBuffer			= this->m_BufferData; //GetBufferData();
					uint32_t iCoreInstance = 0, nMaxCount = (uint32_t)nwol::min(this->Count, (uint32_t)newSize);
					for (; iCoreInstance < nMaxCount; iCoreInstance++)
						((_tRef**)newListBuffer->pByteArray)[iCoreInstance]	= ::nwol::acquire(((_tRef**)oldListBuffer->pByteArray)[iCoreInstance]);
					if (iCoreInstance < newSize)
						memset(&((_tRef**)newListBuffer->pByteArray)[iCoreInstance], 0, sizeof(_tRef*)*(newSize - nMaxCount));
				}
				else
					memset(newListBuffer->pByteArray, 0, m_DataBytes*newSize);

				this->SetBufferData(newListBuffer);
			}
			return 0;
		}

		virtual			::nwol::error_t						pack					()																															{
			glist_nco<_tRef, m_DataType, m_DataBytes>				lstPacked				(this->get_valid_element_count());
			uint32_t												iPacked					= 0;
			for (uint32_t iLocal = 0; iLocal < this->Count; iLocal++) {
				if (this->begin()[iLocal]) {
					lstPacked.begin()[iPacked]							= ::nwol::acquire(this->Data[iLocal]);
					++iPacked;
				}
			}

			(*this) = lstPacked;
			return 0;
		}
		
						::nwol::error_t						push_back				(_tRef* CoreInstance)																										{
			uint32_t												nOldSize				= this->Count;
			::nwol::error_t											errMy					= this->resize(nOldSize + 1);
			reterr_error_if_errored(errMy, "%s", "Failed to resize list! Out of memory?");
			this->set(CoreInstance, nOldSize);
			return 0;
		}
		
						::nwol::error_t						pop						(_tRef** out_pElement = 0)																									{
			reterr_error_if(0 == this->Count	, "%s", "Cannot pop elements from an empty list.");
			reterr_error_if(!this->m_BufferData	, "%s", "Invalid buffer data.");

			_tRef													* oldElement			= 0;
			if (this->m_BufferData.am_I_owning()) {
				if (out_pElement) {
					oldElement										= *out_pElement;
					*out_pElement									= ((_tRef**)this->m_BufferData->pByteArray)[this->m_BufferData->nElementCount = --this->Count];
				}
				else 
					oldElement										= ((_tRef**)this->m_BufferData->pByteArray)[this->m_BufferData->nElementCount = --this->Count];
			}
			else {
				_tRef													* newElement			= 0;
				if (out_pElement)
					newElement											= ::nwol::acquire(((_tRef**)this->m_BufferData->pByteArray)[this->Count - 1]);

				::nwol::error_t											errMy					= this->resize(this->Count - 1);
				if (-1 == errMy) {
					error_printf("%s", "Failed to resize list!");
					return -1;
				}

				if (out_pElement) {
					oldElement											= *out_pElement;
					*out_pElement										= newElement;
				}
				else
					::nwol::release( &newElement );
			}
			::nwol::release(&oldElement);
			return 0;
		}
		
						::nwol::error_t						insert					(_tRef* CoreInstance, uint32_t nIndex)																						{
			reterr_error_if(nIndex >= this->Count, "Invalid index! Index=%u. Max index=%u", nIndex, this->Count - 1);

			uint32_t												oldSize					= this->Count;
			uint32_t												newSize					= this->Count + 1;
			if ((!this->m_BufferData.writable())
			 ||  (this->m_BufferData->nSizeInBytes < (m_DataBytes*newSize))
			)
			{
				GPNCO(::nwol, SBuffer)									newListBuffer;
				reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, newSize, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", newSize);

				uint32_t												iElement;
				for (iElement = 0; iElement < nIndex; iElement++)
					((_tRef**)newListBuffer->pByteArray)[iElement]		= ::nwol::acquire(((_tRef**)this->m_BufferData->pByteArray)[iElement]);
				((_tRef**)newListBuffer->pByteArray)[iElement]		= ::nwol::acquire(CoreInstance);
				iElement++;
				for (; iElement < newSize; iElement++)
					((_tRef**)newListBuffer->pByteArray)[iElement]		= ::nwol::acquire(((_tRef**)this->m_BufferData->pByteArray)[iElement - 1]);
				this->SetBufferData(newListBuffer);
			}
			else {
				for (uint32_t nOldIndex = oldSize; nOldIndex > nIndex; nOldIndex--)
					this->Data[nOldIndex]									= this->Data[nOldIndex - 1];
				this->Data[nIndex]									= ::nwol::acquire(CoreInstance);
				this->m_BufferData->nElementCount					= ++this->Count;
			}
			return 0;
		}
	};	// class

#define GListObj(baseType)				::nwol::glist_obj		<GREF(baseType)>
#define GLstObj(NameSpace, baseType)	::nwol::glist_obj		<NameSpace::GREF(baseType)>
	template<typename _tRef, GDATA_TYPE m_DataType = GTYPEID_POINTER_MAKE(_tRef), int32_t m_DataBytes = GTYPEID_TOTALBYTES(GTYPEID_POINTER_MAKE(_tRef))> class glist_obj : public glist_nco<_tRef, m_DataType, m_DataBytes>
	{
		typedef	glist_nco<_tRef, m_DataType, m_DataBytes>	base_list_type;
		typedef				typename _tRef::TBase			_tBase;
	public:
		inline												glist_obj				()																															{}
		inline												glist_obj				(uint32_t nInitialSize)												: base_list_type::glist_nco(nInitialSize)				{}
															glist_obj				(const ::nwol::gptr_nco<_tRef>* sourceList, uint32_t nElementCount)	: base_list_type::glist_nco(nElementCount)				{
			if (sourceList) {
				for (uint32_t i = 0; i < this->Count; ++i)
					this->Data[i]										= sourceList[i].acquire();
			}
			else {
				error_printf("%s", "Source list is NULL!");
			}
		}

		//
		using				base_list_type::				create;
		virtual				::nwol::error_t					create					(const _tBase* in_lstCoreInstances, uint32_t nInstanceCount)																{
			reterr_error_if(nInstanceCount && 0 == in_lstCoreInstances, "%s", "Cannot create list from a null address!");

			if (this->m_BufferData.writable()
				&& (this->Count >= nInstanceCount || this->m_BufferData->nSizeInBytes >= (nInstanceCount*m_DataBytes)))
			{
				gcreateAll((_tRef**)this->m_BufferData->pByteArray, in_lstCoreInstances, nInstanceCount);
				this->m_BufferData->nElementCount = this->Count		= nInstanceCount;
			}
			else {
				GPNCO(::nwol, SBuffer)									newListBuffer;
				reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nInstanceCount, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", nInstanceCount);
				memset(newListBuffer->pByteArray, 0, nInstanceCount*m_DataBytes);

				gcreateAll((_tRef**)newListBuffer->pByteArray, in_lstCoreInstances, nInstanceCount);
				this->SetBufferData(newListBuffer);
			}
			return 0;
		}

		virtual				::nwol::error_t					create_all				()																															{
			GPNCO(::nwol, SBuffer)									newListBuffer;
			if(this->m_BufferData.writable()) 
				gcreateAll((_tRef**)this->m_BufferData->pByteArray, this->Count);
			else {
				if(this->Count) {
					reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, this->Count, this->m_BufferData->nColumnCount, this->m_BufferData->nSliceCount, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", this->Count);
					memset(newListBuffer->pByteArray, 0, m_DataBytes*this->Count);
					gcreateAll((_tRef**)newListBuffer->pByteArray, this->Count);
				}
				this->SetBufferData(newListBuffer);
			}
			return 0;
		}
	};	// class

	//-------------------------------------------------------------- glist_pod -----------------------------------------------------------------------
#define GListPOD(baseType)				::nwol::glist_pod		<GREF(baseType)>
#define GLstPOD(NameSpace, baseType)	::nwol::glist_pod		<NameSpace::GREF(baseType)>
	template<typename _tRef, GDATA_TYPE m_DataType = GTYPEID_POINTER_MAKE(_tRef), int32_t m_DataBytes = GTYPEID_TOTALBYTES(GTYPEID_POINTER_MAKE(_tRef))> class glist_pod : public glist_obj<_tRef, m_DataType, m_DataBytes>
	{
	public:
		using				glist_obj<_tRef>::				glist_obj;
		typedef				typename _tRef::TBase			_tBase;
		//----------------------------------				-------------
		virtual				::nwol::error_t					serialize				(FILE* fp)																			const									{
			reterr_error_if(0 == fp, "%s", "A null pointer is not a valid file handler!");

			uint32_t												nSize					= this->Count;
			reterr_error_if(1 != fwrite(&nSize, sizeof(uint32_t), 1, fp), "%s", "Failed to write to file!");

			if (0 == nSize)
				return 0;
			uint32_t												nInstancesWritten		= fileSerializeData(this->Data, nSize, fp);
			reterr_error_if(nSize != nInstancesWritten, "Failed to serialize the requested instances! %u requested to be written, %u actually written.", nSize, nInstancesWritten);
			return 0;
		}
		virtual				::nwol::error_t					deserialize				(FILE* fp)																													{
			reterr_error_if(0 == fp, "%s", "A null pointer is not a valid file handler!");

			this->SetBufferData(0);

			uint32_t												nSize					= 0;
			reterr_error_if(1 != fread(&nSize, sizeof(uint32_t), 1, fp), "%s", "Failed to read list from file!");
			if (nSize == 0)
				return 0;

			GPNCO(::nwol, SBuffer)									newListBuffer			= 0;
			reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nSize, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", nSize);
			memset(newListBuffer->pByteArray, 0, m_DataBytes*nSize);

			uint32_t												instancesRead			= fileDeserializeData((_tRef**)newListBuffer->pByteArray, nSize, fp);
			reterr_error_if(nSize != instancesRead, "Failed to read the requested instances! %u requested to be read, %u actually read.", nSize, instancesRead);

			this->SetBufferData(newListBuffer);
			return 0;
		}
		virtual				uint32_t						serialize				(void* out_pByteArray)																const									{
			retnul_error_if(0 == out_pByteArray, "%s", "Cannot serialize to a null address!");

			uint32_t												nSize					= this->Count;
			uint32_t												nBytesWritten			= 0;
			if (out_pByteArray)	
				memcpy(out_pByteArray, &nSize, sizeof(uint32_t));
			nBytesWritten										+= sizeof(uint32_t);
			if (0 == nSize)
				return nBytesWritten;

			nBytesWritten										+= memSerializeData(this->Data, nSize, 0 == out_pByteArray ? 0 : ((char*)out_pByteArray) + nBytesWritten);
			return nBytesWritten;
		}
		virtual				uint32_t						deserialize				(const void* in_pByteArray)																									{
			retnul_error_if(0 == in_pByteArray, "%s", "Cannot deserialize from a null address!");

			this->SetBufferData(0);

			uint32_t												bytesRead				= 0;
			uint32_t												nSize					= 0;
			memcpy(&nSize, in_pByteArray, sizeof(uint32_t));
			bytesRead											+= sizeof(uint32_t);
			if (nSize == 0) {
				return bytesRead;
			}
			GPNCO(::nwol, SBuffer)									newListBuffer;
			if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nSize, &newListBuffer)) {
				error_printf("Cannot create buffer for pointer list of size %u! Out of memory?", nSize);
				return bytesRead;
			}

			memset(newListBuffer->pByteArray, 0, m_DataBytes*nSize);
			bytesRead											+= memDeserializeData((_tRef**)newListBuffer->pByteArray, nSize, ((char*)in_pByteArray) + bytesRead);
			this->SetBufferData(newListBuffer);

			return bytesRead;
		}
		virtual				::nwol::error_t					write					(FILE* fp, const _tBase* defaultData = 0)											const									{
			reterr_error_if(0 == fp, "%s", "A null pointer is not a valid file handler!");

			uint32_t												nSize					= this->Count;
			reterr_error_if(1 != fwrite(&nSize, sizeof(uint32_t), 1, fp), "%s", "Failed to write to file!");

			uint32_t												nInstancesWritten	= fileWriteData(this->Data, nSize, fp, defaultData);
			reterr_error_if(nSize != nInstancesWritten, "Failed to write the requested instances! %u requested to be written, %u actually written.", nSize, nInstancesWritten);
			return 0;
		}
		virtual				::nwol::error_t					read					(FILE* fp)
		{
			reterr_error_if(0 == fp, "%s", "A null pointer is not a valid file handler!");

			this->SetBufferData(0);

			uint32_t												nSize					= ~0U;
			reterr_error_if(1 != fread(&nSize, sizeof(uint32_t), 1, fp), "%s", "Failed to read list from file!");
			if (nSize == 0)
				return 0;

			GPNCO(::nwol, SBuffer)									newListBuffer;
			reterr_error_if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nSize, &newListBuffer), "Cannot create buffer for pointer list of size %u! Out of memory?", nSize);

			memset(newListBuffer->pByteArray, 0, m_DataBytes*nSize);
			uint32_t												instancesRead			= fileReadData((_tRef**)newListBuffer->pByteArray, nSize, fp);
			reterr_error_if(nSize != instancesRead, "Failed to read the requested instances! %u requested to be read, %u actually read.", nSize, instancesRead);
			this->SetBufferData(newListBuffer);
			return 0;
		}
		virtual				uint32_t						write					(void* out_pByteArray, const _tBase* DefaultData = 0)								const									{
			retnul_error_if(0 == out_pByteArray, "%s", "Cannot write to a null address!");
			uint32_t												nSize					= this->Count;
			uint32_t												nBytesWritten			= 0;
			if (out_pByteArray)
				memcpy(out_pByteArray, &nSize, sizeof(uint32_t));

			nBytesWritten										+= sizeof(uint32_t);
			if (0 == nSize)
				return nBytesWritten;
			
			nBytesWritten										+= memWriteData(this->Data, nSize, 0 == out_pByteArray ? 0 : ((char*)out_pByteArray) + nBytesWritten, DefaultData);
			return nBytesWritten;
		}
		virtual				uint32_t						read					(const void* in_pByteArray)																									{
			retnul_error_if(0 == in_pByteArray, "%s", "Cannot read from a null address!");

			this->SetBufferData(0);

			uint32_t												bytesRead				= 0;
			uint32_t												nSize					= 0;
			memcpy(&nSize, in_pByteArray, sizeof(uint32_t));
			bytesRead											+= sizeof(uint32_t);
			if (nSize == 0)
				return bytesRead;

			GPNCO(::nwol, SBuffer)									newListBuffer;
			if(0 > ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, nSize, &newListBuffer)) {
				error_printf("Cannot create buffer for pointer list of size %u! Out of memory?", nSize);
				return bytesRead;
			}

			memset(newListBuffer->pByteArray, 0, m_DataBytes*nSize);
			bytesRead											+= memReadData((_tRef**)newListBuffer->pByteArray, nSize, ((char*)in_pByteArray) + bytesRead);
			this->SetBufferData(newListBuffer);

			return bytesRead;
		}
	};	// class

}	// namespace

#endif // __GLIST_H
