#include "gref_ptr.h"
#include "buffer.h"
//#include <mutex>

#ifndef __GBASELIST_H
#define __GBASELIST_H

namespace nwol
{
	// the following functions are not switable for concurrency:
	// size()
	// get_pointer()
	// get_valid_element_count()
	// release()
	template <typename _tRef, ::nwol::GDATA_TYPE m_DataType = GTYPEID_POINTER_MAKE(_tRef), int32_t m_DataBytes = GTYPEID_TOTALBYTES(GTYPEID_POINTER_MAKE(_tRef))> class glist_base : public ::nwol::array_view<_tRef*>
	{
	protected:
		typedef				typename _tRef::TBase				_tBase;
							GPNCO(::nwol, SBuffer)				m_BufferData;

							void								ReleaseRefList			(::nwol::GODS(SBuffer) oldBuffer)			const			{
			if(oldBuffer && !oldBuffer->shared()) {
				SBuffer														* buf					= oldBuffer->get();
				while (buf->nElementCount)
					::nwol::release(&((_tRef**)buf->pByteArray)[--buf->nElementCount]);
			}
		}

							// Assigns new buffer and releases old references in list if we're the only owner.
							void								SetBufferData			(::nwol::GODS(SBuffer) ListBuffer)							{
			GPNCO(::nwol, SBuffer)										oldBuffer				= this->m_BufferData;	// Catch the old list so we can manage its elements.
			m_BufferData											= ::nwol::acquire(ListBuffer);
			ReleaseRefList(oldBuffer);
			if(ListBuffer) {
				this->Count												= ListBuffer->get()->nElementCount;
				this->Data												= (_tRef**)ListBuffer->get()->pByteArray;
			} 
			else {
				this->Count												= 0;
				this->Data												= 0;
			}
		}

	public:
		// for some reason, clear() override is not being called properly on template destruction.
																~glist_base				()															{ this->SetBufferData(0); }

		inline constexpr										glist_base				()															= default;
		inline													glist_base				(const glist_base& l)
			: array_view<_tRef*>::array_view(l)
		{
			GPNCO(::nwol, SBuffer)										oldBuffer				= this->m_BufferData;	// Catch the old list so we can manage its elements.
			m_BufferData											= l.m_BufferData.acquire();
			this->ReleaseRefList(oldBuffer);
			//SetBufferData(l.m_BufferData);
		}

							_tBase*								operator[]				(uint32_t index)											{
			retnul_error_if(index >= this->Count, "Index out of boundaries! Index: %u. List size: %u.", index, this->Count);
			return this->Data[index] ? this->Data[index]->get() : 0;
		}

							const _tBase*						operator[]				(uint32_t index)							const			{
			retnul_error_if(index >= this->Count, "Index out of boundaries! Index: %u. List size: %u.", index, this->Count);
			return this->Data[index] ? this->Data[index]->get() : 0;
		}
							glist_base&							operator =				(const glist_base<_tRef>& l)								{
			::nwol::array_view<_tRef*>::operator=(l);
			this->SetBufferData(l.m_BufferData);
			return *this;
		}
		
						::nwol::error_t							release					(uint32_t nIndex)											{ set(0, nIndex); return 0; }
						_tRef*									acquire					(uint32_t index)							const			{
			retnul_error_if(index >= this->Count, "Index out of boundaries! Index: %u. List size: %u.", index, this->Count);
			return ::nwol::acquire(this->Data[index]);
		}

		// Checks all elements for valid instances (non-null instance pointers)
						uint32_t								get_valid_element_count	()											const			{
			if(!m_BufferData)
				return 0;

			uint32_t													nValidElementCount		= 0;
			for (uint32_t iElement = 0; iElement < this->Count; iElement++)
				if(((_tRef**)m_BufferData->pByteArray)[iElement])
					++nValidElementCount;

			return nValidElementCount;
		}

						void									clear					()															{
			if(m_BufferData.writable()) {
				this->ReleaseRefList(m_BufferData);
				m_BufferData->nElementCount = this->Count				= 0;
				this->Data												= 0;
			}
			else {
				this->SetBufferData(0);
			}
		}

						::nwol::error_t							release_all				()															{
			GPNCO(::nwol, SBuffer) newListBuffer;
			if(m_BufferData.writable()) {
				_tRef*														* lstRef				= this->begin();
				for( uint32_t i=0; i < this->Count; i++ )
					::nwol::release(&lstRef[i]);
			}
			else {
				if(this->Count) {
					::nwol::error_t												errMy					= ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, this->Count, m_BufferData->nColumnCount, m_BufferData->nSliceCount, &newListBuffer);
					reterr_error_if(0 > errMy, "%s", "Failed to allocate list buffer! Out of memory?");
					memset(newListBuffer->pByteArray, 0, m_DataBytes*this->Count);
				}
				this->SetBufferData(newListBuffer);
			}
			return 0;
		}

						::nwol::error_t							set						(_tRef* CoreInstance, uint32_t nIndex)						{
			reterr_error_if (nIndex >= this->Count, "Invalid index! Index=%u.", nIndex);

			if(m_BufferData.writable()) {
				_tRef														* oldInstance			= ((_tRef**)m_BufferData->pByteArray)[nIndex];
				if (oldInstance != CoreInstance) {
					((_tRef**)m_BufferData->pByteArray)[nIndex]				= ::nwol::acquire(CoreInstance);
					::nwol::release(&oldInstance);
				}
			}
			else {
				GPNCO(::nwol, SBuffer)										newBuffer;
				error_t														errMy					= ::nwol::createBuffer(m_DataType, GUSAGE_POINTER, this->Count, m_BufferData->nColumnCount, m_BufferData->nSliceCount, &newBuffer );
				reterr_error_if(errored(errMy), "%s", "Failed to create buffer for storing new list!");
				for( uint32_t iRef=0; iRef < this->Count; iRef++ )
					((_tRef**)newBuffer->pByteArray)[iRef]					= ::nwol::acquire( ((_tRef**)m_BufferData->pByteArray)[iRef] );

				::nwol::release( &((_tRef**)newBuffer->pByteArray)[nIndex] );
				((_tRef**)newBuffer->pByteArray)[nIndex]				= ::nwol::acquire(CoreInstance);
				this->SetBufferData(newBuffer);
			}
			return 0;
		}
	};
} // namespace

#endif // __GBASELIST_H
