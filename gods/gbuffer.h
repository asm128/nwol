#include "gref_ptr.h"
#include "buffer.h"

#ifndef NWOL_GBUFFER_H_92347
#define NWOL_GBUFFER_H_92347

namespace nwol
{

	//namespace nwol
	//{
	//	template <typename _tRef>	static constexpr inline		uint32_t		writable	( const gptr_nco<_tRef>& in_Ref )		{ return in_Ref && !in_Ref->shared(); };	
	//}

	template<typename _tBase, ::nwol::GDATA_TYPE _F, ::nwol::GDATA_USAGE _USAGE> class gbuffer : public nwol::array_view<_tBase>
	{
	protected:
		GPNCO(::nwol, SBuffer)											m_ArrayBuffer;
		//inline _tBase&												operator[](uint32_t nIndex) { return gbuffer_view<_tBase>::operator[](nIndex);}
	public:
		inline constexpr												gbuffer				()																						= default;
																		gbuffer				(GODS(SBuffer) other)																	{
			if( other ) {
				error_if(other->get()->DataFormat != _F || other->get()->Usage != _USAGE, "%s", "Invalid buffer format!")
				else {
					m_ArrayBuffer = ::nwol::acquire(other);
					this->Data	= (_tBase*)other->get()->pByteArray;
					this->Count	= other->get()->nElementCount;
				}
			}
		}

																		gbuffer				(const gbuffer<_tBase, _F, _USAGE>& other)
			: array_view<_tBase>::array_view(other)
			, m_ArrayBuffer(other.m_ArrayBuffer)
		{
			if( m_ArrayBuffer ) {
				if( _F != m_ArrayBuffer->DataFormat ) {
					error_printf("%s", "Invalid element format!");
					m_ArrayBuffer.free();
					this->Count = 0;
				}
				else if( _USAGE != m_ArrayBuffer->Usage ) {
					error_printf("%s", "Invalid element usage!");
					m_ArrayBuffer.free();
					this->Count = 0;
				}
			}
		}

		inline															gbuffer				(uint32_t newSize)																		{ throw_if(0 > resize(newSize), "", "%s", "Out of memory!"); }

		template <size_t _Size> 
		inline															gbuffer				(const _tBase (&_Dst)[_Size])															: gbuffer(_Size, _Dst)							{}
																		gbuffer				(uint32_t size, const _tBase* valueList)												{
			throw_if(errored(resize(size)), "", "%s", "Out of memory!")
			else if(valueList) {
				for(uint32_t i=0; i<size; i++)
					set_value(i, valueList[i]);
			}
		}

		//inline const _tBase&			operator[]			(uint32_t nIndex) const { return array_view<_tBase>::operator[](nIndex);}
 
								GODS(SBuffer)							GetBufferData		()																	const				{ return m_ArrayBuffer.acquire();				}

		static inline constexpr	uint32_t								element_size		()																						{ return GTYPEID_TOTALBYTES(_F);				}
		inline constexpr		uint32_t								size_in_bytes		()																	const	noexcept	{ return this->Count*(uint32_t)sizeof(_tBase);	}
		inline constexpr		const _tBase&							get_value			( uint32_t index )													const				{ return this->Data[index];						}
								::nwol::error_t							set_value			( uint32_t index, const _tBase& value )
		{
			ree_if(index >= this->size(), "Invalid index: #%u!", index);
			if(m_ArrayBuffer.writable()) {
				this->Data[index]												= value;
				return 0;
			}

			static constexpr const bool											bIsText				= _USAGE == GUSAGE_TEXT;

			GPNCO(::nwol, SBuffer)												newListBuffer;
			nwol_necall(::nwol::createBuffer(_F, _USAGE, this->Count, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)this->Count);
		
			memcpy(newListBuffer->pByteArray, this->Data, sizeof(_tBase)*this->Count);
			((_tBase*)newListBuffer->pByteArray)[index]						= value;

			if(bIsText)
				memset( &((_tBase*)newListBuffer->pByteArray)[this->Count], 0, sizeof(_tBase) );

			set(newListBuffer);
			return 0;
		}

								void									clear				()																						{
			this->Count														= 0;
			this->Data														= 0;
			if(m_ArrayBuffer.writable())
				m_ArrayBuffer->nElementCount									= 0;
			else
				m_ArrayBuffer.free();
		}

								void									set					( GODS(SBuffer) InputBuffer )															{
			if( 0 == InputBuffer ) {
				this->Data														= 0;
				this->Count														= 0;
			}
			else error_if(InputBuffer->get()->DataFormat != _F, "%s", "Invalid buffer format!")
			else error_if(InputBuffer->get()->Usage != _USAGE, "%s", "Invalid buffer usage!")
			else {
				m_ArrayBuffer													= ::nwol::acquire( InputBuffer );
				this->Data														= (_tBase*)m_ArrayBuffer->pByteArray;
				//Count = m_ArrayBuffer->nElementCount; we don't do this because the count is assigned outside.
			}
		}

								int32_t									find				( const _tBase& value ) const															{
			for( uint32_t iElement=0; iElement < this->Count; iElement++ ) {
				if( 0 == memcmp(&this->Data[iElement], &value, sizeof(_tBase) ) )
					return iElement;
			}
			return -1;
		}

								int32_t									rfind				( const _tBase& value ) const															{
			for( int32_t iElement=((int32_t)this->Count)-1; iElement>=0; --iElement ) {
				if( 0 == memcmp(&this->Data[iElement], &value, sizeof(_tBase) ) )
					return iElement;
			}
			return -1;
		}

								int32_t									replace_first		( const _tBase& oldValue, const _tBase& newValue )										{
			if( 0 == memcmp(&oldValue, &newValue) )
				return -1;

			int32_t																iFirst				= find(oldValue); 
			if( -1 != iFirst ) {
				if( m_ArrayBuffer.writable() )
					this->Data[iFirst]												= newValue;
				else {
					GPNCO(::nwol, SBuffer)												newArray;
					nwol_necall(cloneBuffer(&newArray, m_ArrayBuffer), "Unknown error.");
					((_tBase*)newArray->pByteArray)[iFirst]							= newValue;
					set(newArray);
				}
			}
			return iFirst;
		}

								int32_t									replace_last		( const _tBase& oldValue, const _tBase& newValue )										{
			if( 0 == memcmp(&oldValue, &newValue) )
				return -1;

			int32_t																iLast				= rfind(oldValue);
			if( -1 != iLast )  {
				if( m_ArrayBuffer.writable() )
					this->Data[iLast]												= newValue;
				else {
					GPNCO(::nwol, SBuffer)												newArray;
					nwol_necall(cloneBuffer(&newArray, m_ArrayBuffer), "Unknown error.");
					ree_if(0 == newArray || 0 == newArray->pByteArray, "Failed to allocate new buffer.");
					((_tBase*)newArray->pByteArray)[iLast]							= newValue;
					set(newArray);
				}
			}
			return iLast;
		}

								int32_t									replace				( const _tBase& oldValue, const _tBase& newValue )										{
			if( 0 == memcmp(&oldValue, &newValue) )
				return 0;

			uint32_t															nReplacedCount		= 0;
			
			if( m_ArrayBuffer.writable() ) {
				for( uint32_t iEl=0; iEl< this->Count; iEl++ ) {
					if( 0 == memcmp(&this->Data[iEl], &oldValue, sizeof(_tBase)) ) {
						memcpy(&this->Data[iEl], &newValue, sizeof(_tBase));
						++nReplacedCount;
					}
				}
			}
			else {
				uint32_t															firstOccurrence		= find(oldValue);
				if( -1 != firstOccurrence ) {
					GPNCO(::nwol, SBuffer)												newArray;
					nwol_necall(cloneBuffer(&newArray, m_ArrayBuffer), "Unknown error.");
					for( uint32_t iEl=firstOccurrence; iEl < this->Count; iEl++ ) {
						_tBase																* pElArray		= (_tBase*)newArray->pByteArray;
						if( 0 == memcmp(&pElArray[iEl], &oldValue, sizeof(_tBase)) ) {
							memcpy(&pElArray[iEl], &newValue, sizeof(_tBase));
							++nReplacedCount;
						}
					}
					this->set(newArray);
				}
			}
			return nReplacedCount;
		}

								::nwol::error_t							resize				(uint32_t newSize)																		{
			if(this->Count == newSize)
				return 0;
			else if(0 == newSize) {
				clear();
				return 0;
			}

			static constexpr const bool											bIsText				= _USAGE == GUSAGE_TEXT;

			if( m_ArrayBuffer.writable() 
				&& ((newSize+one_if(bIsText))*sizeof(_tBase)) <= m_ArrayBuffer->nSizeInBytes )
			{
				m_ArrayBuffer->nElementCount = this->Count								= newSize;
				this->Data																= (_tBase*)m_ArrayBuffer->pByteArray;
				return 0;
			}

			GPNCO(::nwol, SBuffer)												newListBuffer;
			nwol_necall(::nwol::createBuffer(_F, _USAGE, (uint32_t)(newSize + one_if(bIsText)), &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);

			uint32_t															copyCount			= nwol::min(this->Count, newSize);
			if(copyCount)
				memcpy(newListBuffer->pByteArray, this->Data, sizeof(_tBase)*copyCount);

			if(bIsText)
				memset( &((_tBase*)newListBuffer->pByteArray)[newSize], 0, sizeof(_tBase) );

			newListBuffer->nElementCount = this->Count						= newSize;
			set(newListBuffer);
			return 0;
		}

								::nwol::error_t							push_back			(const _tBase& value)																	{
			uint32_t															oldSize				= this->Count;
			uint32_t															newSize				= oldSize+1;
			nwol_necall(resize(newSize), "%s", "Cannot resize array! Out of memory?");
			if(m_ArrayBuffer.writable())
				this->Data[oldSize]													= value;
			else {	// how does this happen?
				PLATFORM_CRT_BREAKPOINT();
				GPNCO(::nwol, SBuffer)												newListBuffer;
				if(m_ArrayBuffer) {
					nwol_necall(::nwol::createBuffer(_F, _USAGE, newSize, m_ArrayBuffer->nColumnCount, m_ArrayBuffer->nSliceCount, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);
					memcpy(newListBuffer->pByteArray, this->Data, oldSize);
				}
				else {	// Entering here doesn't make any sense either if we resized()+1 successfully in a previous statement.
					PLATFORM_CRT_BREAKPOINT();
					nwol_necall(::nwol::createBuffer(_F, _USAGE, newSize, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);
				}
				((_tBase*)newListBuffer->pByteArray)[oldSize]					= value;
				set(newListBuffer);
			}
			return 0;
		}

								::nwol::error_t							pop						( _tBase* out_value )																	{
			uint32_t															newSize					= this->Count-1;
			_tBase																value					= this->Data[newSize];
			nwol_necall(resize(newSize), "%s", "Cannot resize array! Out of memory?");
			if( out_value )
				*out_value													= value;
			return 0;
		}

								::nwol::error_t							insert					(uint32_t nIndex, const _tBase& value)													{
			ree_if(nIndex >= this->size(), "Invalid index! Index=%u. Max index=%u", nIndex, this->size()-1);
			uint32_t															nOldSize				= this->Count;
			uint32_t															newSize					= this->Count+1;

			static const bool													bIsText					= _USAGE == GUSAGE_TEXT;
			if( m_ArrayBuffer.writable() 
			 && m_ArrayBuffer->nSizeInBytes >= (sizeof(_tBase)*(newSize+one_if(bIsText)))
			 )
			{
				for(uint32_t nOldIndex = nOldSize; nOldIndex>nIndex; nOldIndex--)
					this->Data[nOldIndex]											= this->Data[nOldIndex-1];
				this->Data[nIndex]												= value;
				m_ArrayBuffer->nElementCount = this->Count						= newSize;
				return 0;
			}

			// WARNING! Unsafe pointer handling in course.
			GPNCO(::nwol, SBuffer)												newListBuffer
				,																oldListBuffer			(m_ArrayBuffer)
				;
			nwol_necall(::nwol::createBuffer(_F, _USAGE, newSize, oldListBuffer->nColumnCount, oldListBuffer->nSliceCount, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);

			uint32_t															iElement				= 0;
			for(iElement=0; iElement < nIndex; iElement++)
				((_tBase*)newListBuffer->pByteArray)[iElement]					= this->Data[iElement];
			((_tBase*)newListBuffer->pByteArray)[iElement]					= value;
			iElement++;
			for( ; iElement < newSize; iElement++)
				((_tBase*)newListBuffer->pByteArray)[iElement]					= this->Data[iElement-1];
			
			set(newListBuffer);
			this->Count														= newSize;
			return 0;
		}

								::nwol::error_t							remove					(uint32_t nIndex)																		{
			ree_if(nIndex >= this->size(), "Invalid index! Index=%u. Max index=%llu", nIndex, (uint64_t)this->size()-1);

			uint32_t															newSize					= this->Count-1;
			static constexpr const bool											bIsText					= _USAGE == GUSAGE_TEXT;
			if( m_ArrayBuffer.writable() ) {
				for( uint32_t nOldIndex = nIndex; nOldIndex<newSize; nOldIndex++ )
					this->Data[nOldIndex]												= this->Data[nOldIndex+1];

				m_ArrayBuffer->nElementCount = this->Count						= newSize;
				return 0;
			}

			// WARNING! Unsafe pointer handling in course.
			GPNCO(::nwol, SBuffer)	newListBuffer;
			nwol_necall(::nwol::createBuffer(_F, _USAGE, newSize, m_ArrayBuffer->nColumnCount, m_ArrayBuffer->nSliceCount, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);

			uint32_t															iElement;

			for( iElement=0; iElement < nIndex; iElement++ )
				((_tBase*)newListBuffer->pByteArray)[iElement]					= this->Data[iElement];

			for( ; iElement < newSize; iElement++ )
				((_tBase*)newListBuffer->pByteArray)[iElement]					= this->Data[iElement+1];

			set(newListBuffer);
			this->Count														= newSize;

			return 0;
		}

								::nwol::error_t							reverse					()																						{
			if( 0 == this->Count )
				return 0;

			if( m_ArrayBuffer.writable() ) {
				_tBase																temp; 
				bool																isOdd					= true_if(this->Count % 2);
					
				for(uint32_t iElement = 0, nCount = (isOdd ? this->Count - 1 : this->Count) / 2; iElement < nCount; ++iElement) {
					uint32_t															rightIndex				= this->Count - 1 - iElement;
					temp															= this->Data[iElement];
					this->Data[iElement]											= this->Data[rightIndex];
					this->Data[rightIndex]											= temp;
				}
			}
			else {
				GPNCO(::nwol, SBuffer)												newListBuffer;
				nwol_necall(::nwol::createBuffer(_F, _USAGE, this->Count, m_ArrayBuffer->nColumnCount, m_ArrayBuffer->nSliceCount, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)this->Count);

				_tBase																* newData				= (_tBase*)newListBuffer->pByteArray;
				for( uint32_t iElement=0; iElement<this->Count; iElement++ )
					newData[iElement]												= this->Data[this->Count - 1 - iElement];

				set(newListBuffer);
			}
			return 0;
		}

								::nwol::error_t							reserve					(uint32_t size)																			{
			GPNCO(::nwol, SBuffer)												newListBuffer;
			if( !m_ArrayBuffer ) {
				nwol_necall(::nwol::createBuffer(_F, _USAGE, size, size, 1, &newListBuffer), "%s", "Failed to reserve buffer! Out of memory?");
				newListBuffer->nElementCount									= 0;
				m_ArrayBuffer													= newListBuffer;
				return 0;
			}
			else if( m_ArrayBuffer.writable() && (m_ArrayBuffer->nSizeInBytes >= (size*sizeof(_tBase))))
				return 0;

			GPNCO(::nwol, SBuffer)												oldBuffer				= m_ArrayBuffer;
			nwol_necall(::nwol::createBuffer(_F, _USAGE, size, oldBuffer->nColumnCount, oldBuffer->nSliceCount, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)size);

			newListBuffer->nElementCount									= this->Count;
			memcpy(newListBuffer->pByteArray, oldBuffer->pByteArray, sizeof(_tBase)*this->Count);
			set(newListBuffer);

			return 0;
		}

		// Set this instance to the list resulting from joining _A with _B
								::nwol::error_t							join						(const gbuffer<_tBase, _F, _USAGE>& _a, const gbuffer<_tBase, _F, _USAGE>& _b)					{
			GPNCO(::nwol, SBuffer)												newListBuffer;
			nwol_necall(joinBuffers( _a.m_ArrayBuffer, _b.m_ArrayBuffer, &newListBuffer ), "What happened?");
			set(newListBuffer);
			this->Count														= _a.size()+_b.size();
			return 0;
		}

								::nwol::error_t							save						( FILE* fp )															const			{ return (1 == fileSerializeData( &m_ArrayBuffer, 1, fp )) ? 0 : -1;		}
								uint32_t								save						( char* fp )															const			{ return memSerializeData( &m_ArrayBuffer, 1, fp );							}

								::nwol::error_t							load						( FILE* fp )																			{
			::nwol::error_t														errMy						= (1 == fileDeserializeData( &m_ArrayBuffer, 1, fp )) ? 0 : -1;
			if( m_ArrayBuffer ) {
				this->Count														= m_ArrayBuffer->nElementCount;
				this->Data														= (_tBase*)m_ArrayBuffer->pByteArray;
			}
			return errMy ? -1 : 0;
		}

								uint32_t								load						( const char* fp )																		{
			::nwol::error_t														errMy						= memDeserializeData( &m_ArrayBuffer, 1, fp );
			if( m_ArrayBuffer ) {
				this->Count														= m_ArrayBuffer->nElementCount;
				this->Data														= (_tBase*)m_ArrayBuffer->pByteArray;
			}
			return errMy ? -1 : 0;
		}
	};

	// We should add overload methods in this class for initializing to null the array elements after the buffer is created or resized.
	template<typename _tBase> class gapointer : public gbuffer<_tBase*, ::nwol::GDATA_TYPE_POINTER, GUSAGE_POINTER>
	{
	public:
		using gbuffer<_tBase*, ::nwol::GDATA_TYPE_POINTER, GUSAGE_POINTER>::gbuffer;
	};

	// Here it would be nice to pack the booleans into bit fields.
	class gabool : public gbuffer<bool, ::nwol::GDATA_TYPE_UINT1, GUSAGE_BOOLEAN>
	{
	public:
		using gbuffer<bool, ::nwol::GDATA_TYPE_UINT1, GUSAGE_BOOLEAN>::gbuffer;
	};

	// Binary string type
	typedef gbuffer<ubyte_t,			GDATA_TYPE_INT8,			GUSAGE_BINARY>									gabstring;
	//typedef gbuffer<ubyte_t,			GDATA_TYPE_UINT8,			GUSAGE_BINARY>									gabstring;

	// Index lists of different sizes.
	typedef gbuffer<uint8_t,		GDATA_TYPE_UINT8,			GUSAGE_INDEX>									gauindex8;
	typedef gbuffer<uint16_t,		GDATA_TYPE_UINT16,			GUSAGE_INDEX>									gauindex16;
	typedef gbuffer<uint32_t,		GDATA_TYPE_UINT32,			GUSAGE_INDEX>									gauindex32;
	typedef gbuffer<uint64_t,		GDATA_TYPE_UINT64,			GUSAGE_INDEX>									gauindex64;
	typedef gbuffer<int8_t,			GDATA_TYPE_INT8,			GUSAGE_INDEX>									gaindex8;
	typedef gbuffer<int16_t,		GDATA_TYPE_INT16,			GUSAGE_INDEX>									gaindex16;
	typedef gbuffer<int32_t,		GDATA_TYPE_INT32,			GUSAGE_INDEX>									gaindex32;
	typedef gbuffer<int64_t,		GDATA_TYPE_INT64,			GUSAGE_INDEX>									gaindex64;

	// These are useful for building other kind of buffers.
	typedef gbuffer<uint32_t,		GDATA_TYPE_UINT32,			GUSAGE_VERTEXCOLOR>								gavertexcolor;
	typedef gbuffer<uint32_t,		GDATA_TYPE_UINT8_4,			GUSAGE_TEXEL>									gatexel;
	typedef gbuffer<uint16_t,		GDATA_TYPE_UINT16,			GUSAGE_TEXEL>									gatexel16;
	typedef gbuffer<uint8_t,		GDATA_TYPE_UINT8,			GUSAGE_TEXEL>									gatexel8;
	typedef gbuffer<GDATA_TYPE,		GDATA_TYPE_DATATYPE,		GUSAGE_DATATYPE>								gadatatype;
	typedef gbuffer<GDATA_USAGE,	GDATA_TYPE_RESOURCEUSAGE,	GUSAGE_USAGE>									gausage;
	typedef gbuffer<STypeUsage,		GDATA_TYPE_TYPEUSAGE,		GDATA_USAGE(GUSAGE_DATATYPE | GUSAGE_USAGE)>	gatypeusage;

	// These are useful for native type buffers.
	typedef gbuffer<char,			GDATA_TYPE_INT8,			GUSAGE_SCALAR>									gachar, gabyte;
	typedef gbuffer<short,			GDATA_TYPE_INT16,			GUSAGE_SCALAR>									gashort;
	typedef gbuffer<int,			GDATA_TYPE_INT32,			GUSAGE_SCALAR>									gaint;
	typedef gbuffer<unsigned char,	GDATA_TYPE_UINT8,			GUSAGE_SCALAR>									gauchar, gaubyte;
	typedef gbuffer<unsigned short,	GDATA_TYPE_UINT16,			GUSAGE_SCALAR>									gaushort;
	typedef gbuffer<unsigned int,	GDATA_TYPE_UINT32,			GUSAGE_SCALAR>									gauint;
	typedef gbuffer<float,			GDATA_TYPE_FLOAT32,			GUSAGE_SCALAR>									gafloat;
	typedef gbuffer<double,			GDATA_TYPE_FLOAT64,			GUSAGE_SCALAR>									gadouble;
	//typedef gbuffer<unsigned float,	GDATA_TYPE_FLOAT32,		GUSAGE_BINARY>									gafloat;
	// Custom types.
	typedef gbuffer<id_t,			GDATA_TYPE_INT32,			GUSAGE_INDEX>									gaid;
	typedef gbuffer<::nwol::glabel,	GTYPEID_DATA_MAKE(::nwol::glabel),	GUSAGE_BINARY>							galabel;
};

#endif // NWOL_GBUFFER_H_92347
