#include "gbuffer.h"

#ifndef __GSTRING_H_1654987
#define __GSTRING_H_1654987

namespace nwol
{
	// The difference between this template and the base gbuffer<_tBase, _F, GUSAGE_TEXT> type is that this class handles the null character at the end of the string
	template<typename _tBase, ::nwol::GDATA_TYPE _F> class gestring : public gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>	
	{
	public:
		using			gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::		m_ArrayBuffer;
		using			gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::		Data;
		using			gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::		Count;

		using			gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::		size;
		using			gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::		set;

		inline															gestring			()									= default;
		inline															gestring			(const gestring& other)				: gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>(other)		{}
		inline															gestring			(uint32_t size)						: gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>(size)		{}
		template <size_t _S>											gestring			(const _tBase (&data)[_S])			: gestring(_S, data)									{}
																		gestring			(uint32_t size, const _tBase* data)															{
			GPNCO(::nwol, SBuffer)												newListBuffer;
			if( 0 > ::nwol::createBuffer(_F, ::nwol::GUSAGE_TEXT, size+1, &newListBuffer) ) {
				error_printf("Failed to create string buffer of size %u.", size+1);
			}
			else {
				uint32_t															i					= 0;
				_tBase																* newBaseArray		= (_tBase*)newListBuffer->pByteArray;
				for( uint32_t nCount=size; i<nCount && (0 != data[i]); i++)
					newBaseArray[i]													= data[i];

				newBaseArray[newListBuffer->nElementCount = Count = i]			= 0;
				set(newListBuffer);
			}
		}
		template <size_t _S>				gestring&					operator+=			(const _tBase (&data)[_S])																	{ append(data);												return *this;	}
		inline								gestring&					operator+=			(const gestring<_tBase, _F>& other)															{ append(other);											return *this;	}
		template <size_t _S>				gestring					operator+			(const _tBase (&data)[_S])												const				{ gestring<_tBase, _F> result = *this; result.append(data) ; return result; }
		inline								gestring					operator+			(const gestring<_tBase, _F>& other)										const				{ gestring<_tBase, _F> result = *this; result.append(other); return result; }
		inline constexpr					bool						operator!			()																		const	noexcept	{ return 0 == Count;														}
		inline constexpr					operator					bool				()																		const	noexcept	{ return 0 != Count;														}
											void						clear				()																							{
			if(this->m_ArrayBuffer.writable()) {
				if( Count )
					memset(&Data[0], 0, sizeof(_tBase));
				this->m_ArrayBuffer->nElementCount								= 0;
			}
			else
				set(0);
			Count															= 0;
		}
											::nwol::error_t				join				(const _tBase* _A, uint32_t _ASize, const _tBase* _B, uint32_t _BSize)						{
			bool																leftIsEmpty			= (0 == _ASize) || (_A[0] == 0);
			bool																rightIsEmpty		= (0 == _BSize) || (_B[0] == 0);

			if( leftIsEmpty && rightIsEmpty )
				clear();
			else if( leftIsEmpty )
				*this															= gestring<_tBase, _F>(_BSize, _B);
			else if( rightIsEmpty )
				*this															= gestring<_tBase, _F>(_ASize, _A);
			else {
				GPNCO(::nwol, SBuffer)												newListBuffer;
				uint32_t															newSize				= _ASize+_BSize+1;
				ree_if(0 > ::nwol::createBuffer(_F, ::nwol::GUSAGE_TEXT, newSize, &newListBuffer), "Failed to create buffer for array! Out of memory? Element count requested: %u", (uint32_t)newSize);
				_tBase																* newBaseArray		= ((_tBase*)newListBuffer->pByteArray);
				uint32_t															totalChars			= 0;
				for( uint32_t i=0, nCount=_ASize; i<nCount && 0 != _A[i]; i++)
					newBaseArray[totalChars++]										= _A[i];

				for( uint32_t i=0, nCount=_BSize; i<nCount && 0 != _B[i]; i++)
					newBaseArray[totalChars++]										= _B[i];

				newBaseArray[newListBuffer->nElementCount = Count = totalChars]	= 0;
				set(newListBuffer);
			}
			return 0;
		}
											::nwol::error_t				join				( const gestring<_tBase, _F>& _A, const gestring<_tBase, _F>& _B )							{
			bool																leftIsEmpty			= 0 == _A.size();
			bool																rightIsEmpty		= 0 == _B.size();

			if( leftIsEmpty && rightIsEmpty )
				clear();
			else if( leftIsEmpty )
				*this															= _B;
			else if( rightIsEmpty )
				*this															= _A;
			else
				return join( _A.begin(), _A.size(), _B.begin(), _B.size() );

			return 0;
		}
		template <size_t _S>				::nwol::error_t				join				( const gestring<_tBase, _F>& _A, const _tBase (&_B)[_S] )									{
			const bool															leftIsEmpty			= 0 == _A.size();
			const bool															rightIsEmpty		= 0 == _S || (_B[0] == 0);

			if( leftIsEmpty && rightIsEmpty )	clear();
			else if( leftIsEmpty	)			*this							= _B;
			else if( rightIsEmpty	)			*this							= _A;
			else
				return join( _A.begin(), _A.size(), _B, _S );

			return 0;
		}
		template <size_t _S>				::nwol::error_t				join				( const _tBase (&_A)[_S], const gestring<_tBase, _F>& _B )									{
			const bool															leftIsEmpty			= 0 == _S || (_A[0] == 0);
			const bool															rightIsEmpty		= 0 == _B.size();

			if( leftIsEmpty && rightIsEmpty )	clear();
			else if( leftIsEmpty	)			*this							= _B;
			else if( rightIsEmpty	)			*this							= _A;
			else
				return join( _A, _S, _B.begin(), _B.size() );
			return 0;
		}
		template <size_t _S1, size_t _S2>	::nwol::error_t				join				( const _tBase (&_A)[_S1], const _tBase (&_B)[_S2] )										{
			const bool															leftIsEmpty			= 0 == _S1 || (_A[0] == 0);
			const bool															rightIsEmpty		= 0 == _S2 || (_B[0] == 0);

			if( leftIsEmpty && rightIsEmpty )	clear();
			else if( leftIsEmpty )				*this							= _B;
			else if( rightIsEmpty )				*this							= _A;
			else
				return join( _A, _S1, _B, _S2 );
			return 0;
		}
		inline								::nwol::error_t				push_back			( const _tBase& element )																	{ return append(element); }
											::nwol::error_t				append				( const _tBase& element )																	{
			bool																leftIsEmpty			= 0 == Count;
			if( element != 0 ) {
				if( leftIsEmpty ) {
					if( this->m_ArrayBuffer.writable() && this->m_ArrayBuffer->nSizeInBytes >= sizeof(element)*2 ) {
						Data[Count++]												= element;	// Count should be zero when size() returns 0
						Data[this->m_ArrayBuffer->nElementCount = Count]			= 0;
					}
					else
						*this														= gestring<_tBase, _F>(1, &element);
				}
				else
					return append(&element, 1);
			}
			return 0;
		}	
		template <size_t _S>				::nwol::error_t				append				( const _tBase (&_B)[_S] )																	{
			bool																leftIsEmpty			= 0 == size();
			if( _S && (_B[0] != 0) ) {
				if( leftIsEmpty ) {
					if( this->m_ArrayBuffer.writable() && this->m_ArrayBuffer->nSizeInBytes >= sizeof(_tBase)*(Count+_S) ) {
						for( uint32_t iElement=0; iElement < _S && _B[iElement] != 0; ++iElement )
							Data[Count++]													= _B[iElement];

						Data[this->m_ArrayBuffer->nElementCount = Count]				= 0;
					}
					else
						*this															= gestring<_tBase, _F>(_B);
				}
				else
					return append(&_B[0], (uint32_t)_S);
			}
			return 0;
		}
											::nwol::error_t				append				( const gestring<_tBase, _F>& _B )															{
			bool																leftIsEmpty			= 0 == size();
			uint32_t															sizeB				= _B.size();

			if( sizeB ) {
				if( leftIsEmpty )
					*this															= _B;
				else
					return append( _B.begin(), sizeB );
			}
			return 0;
		}
											::nwol::error_t				append				( const _tBase* _B, uint32_t nSize )														{
			if( nSize ) {
				uint32_t															leftSize			= size();
				if( 0 == leftSize ) {
					uint32_t															totalSize			= (nSize+1)*GTYPEID_TOTALBYTES(_F);
					if( this->m_ArrayBuffer.writable() && this->m_ArrayBuffer->nSizeInBytes >= totalSize ) {
						uint32_t															iChar				= 0;
						for( ; (iChar<nSize) && (_B[iChar] != 0); iChar++ )
							Data[iChar]														= _B[iChar];
						this->m_ArrayBuffer->nElementCount = Count						= leftSize+iChar;
						Data[Count]														= 0;
					}
					else 
						*this															= gestring<_tBase, _F>(nSize, _B);
				}
				else {
					uint32_t															totalSize			= (leftSize+nSize+1)*GTYPEID_TOTALBYTES(_F);
					if( this->m_ArrayBuffer.writable() && this->m_ArrayBuffer->nSizeInBytes >= totalSize ) {
						uint32_t															iChar				= 0;
						_tBase																* pArray			= &Data[leftSize];
						for( ; (iChar<nSize) && (_B[iChar] != 0); ++iChar )
							pArray[iChar]													= _B[iChar];
						Count															= leftSize+iChar;
						Data[this->m_ArrayBuffer->nElementCount = Count]				= 0;
					}
					else
						return join(Data, leftSize, _B, nSize);
				}
			}
			return 0;
		}
		template <size_t _S>				::nwol::error_t				prepend				( const _tBase (&_B)[_S] )																	{ ::nwol::gestring<_tBase, _F> result(_B)			; if( 0 > result.append(*this) ) return -1; *this = result; return 0; }
											::nwol::error_t				prepend				( const gestring<_tBase, _F>& _B )															{ ::nwol::gestring<_tBase, _F> result(_B)			; if( 0 > result.append(*this) ) return -1; *this = result; return 0; }
											::nwol::error_t				prepend				( const _tBase* _B, uint32_t nSize )														{ ::nwol::gestring<_tBase, _F> result(nSize, _B)	; if( 0 > result.append(*this) ) return -1; *this = result; return 0; }
											int32_t						replace_first		( const _tBase& oldValue, const _tBase& newValue )											{
			int32_t																iFirst				= gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::replace_first(oldValue);

			if( -1 != iFirst && 0 == newValue )
				this->m_ArrayBuffer->nElementCount = Count						= iFirst;

			return iFirst;
		}
											int32_t						replace_last		( const _tBase& oldValue, const _tBase& newValue )											{
			int32_t																iLast				= gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::replace_last(oldValue);
			if( -1 != iLast && 0 == newValue ) 
				this->m_ArrayBuffer->nElementCount = Count					= iLast;

			return iLast;
		}
											int32_t						replace				( const _tBase& oldValue, const _tBase& newValue )											{
			if( 0 == newValue )
				return one_if(replace_first(oldValue, newValue) != -1);
			else 
				return gbuffer<_tBase, _F, ::nwol::GUSAGE_TEXT>::replace(oldValue, newValue);
		}
											::nwol::error_t				load				( FILE* fp )																				{
			::nwol::error_t														result				= (1 == fileDeserializeData( &this->m_ArrayBuffer, 1, fp )) ? 0 : -1;
			if( this->m_ArrayBuffer ) {
				Data														= (_tBase*)this->m_ArrayBuffer->pByteArray;
				Data[Count = this->m_ArrayBuffer->nElementCount]			= 0;
			}
			return result;
		}
											uint32_t					load				( const char* fp )																			{
			uint32_t															nBytesRead			= memDeserializeData( &this->m_ArrayBuffer, 1, fp );
			if( this->m_ArrayBuffer ) {
				Data														= (_tBase*)this->m_ArrayBuffer->pByteArray;
				Data[Count = this->m_ArrayBuffer->nElementCount]			= 0;
			}
			return nBytesRead;
		}
	};	// struct

	class gacstring : public gestring<char		, ::nwol::GDATA_TYPE_UINT8>				{	public:	using gestring<char		, ::nwol::GDATA_TYPE_UINT8	>::gestring; };
	class gewstring : public gestring<wchar_t	, ::nwol::GDATA_TYPE_UINT16>			{	public:	using gestring<wchar_t	, ::nwol::GDATA_TYPE_UINT16	>::gestring; };

}

#endif // __GSTRING_H_1654987
