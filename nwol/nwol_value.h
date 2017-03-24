#ifndef __NWOL_VALUE_H__92837423__
#define __NWOL_VALUE_H__92837423__

namespace nwol
{
	template <typename _T>					static inline const _T&				get_value_at_offset			( const void * bytes, uint32_t offset )								{ return *reinterpret_cast<const	_T*>(&((const	char*)bytes)[offset]);						}
	template <typename _T>					static inline _T&					get_value_at_offset			(		void * bytes, uint32_t offset )								{ return *reinterpret_cast<			_T*>(&((		char*)bytes)[offset]);						}
	template <typename _T>					static inline void					set_value_at_offset			(		void * bytes, uint32_t offset, const	_T & value )		{ *reinterpret_cast<_T*>(&((char*)bytes)[offset]) = value;										}
	template <typename _T>					static inline void					memcpy_value_from_offset	( const void * bytes, uint32_t offset,			_T & value )		{ const _T* at = (const _T*)&((const	char*)bytes)[offset]; memcpy(&value, at, sizeof(_T));	}
	template <typename _T>					static inline void					memcpy_value_to_offset		(		void * bytes, uint32_t offset, const	_T & value )		{		_T* at = (		_T*)&((			char*)bytes)[offset]; memcpy(at, &value, sizeof(_T));	}

	template <typename _T>					static inline uint32_t				get_value_index				( const _T& value, const _T* arrayValues, uint32_t nMaxCount )		{
		for( uint32_t i=0; i<nMaxCount; i++ )
			if( arrayValues[i] == value )
				return i;

		return -1;
	}
	template <typename _T, size_t _Size>	static inline uint32_t				get_value_index				( const _T& value, const _T (&arrayValues)[_Size] )					{
		for( uint32_t i=0; i<_Size; ++i )
			if( arrayValues[i] == value )
				return i;

		return -1;
	}
	template<typename _T, size_t _Size>		static inline _T*					array_copy					( _T (&targetArray)[_Size], _T (&sourceArray)[_Size] )				{																								
		for( uint32_t i=0; i < _Size; ++i	)											
			targetArray[i]																	= sourceArray[i];																		

		return targetArray;																				
	}
	template<typename _T, size_t _Size>		static inline _T*					grid_copy					( _T (&targetGrid)[_Size][_Size], _T (&sourceGrid)[_Size][_Size] )	{																								
		for( uint32_t z=0; z < _Size; ++z	)											
			for( uint32_t x=0; x < _Size; ++x	)											
				targetGrid[z][x]																= sourceGrid[z][x];																		

		return &targetGrid[0][0];																				
	}
	template<typename _T, size_t _Size>		static inline _T*					grid_transpose				( _T (&targetGrid)[_Size][_Size], _T (&sourceGrid)[_Size][_Size] )	{
		for( uint32_t z=0; z < _Size; z++ )
			for( uint32_t x=0; x < _Size; x++ )
				targetGrid[x][z]															= sourceGrid[z][x];

		return &targetGrid[0][0];																		
	}
	template<typename _T, size_t _Size>		static inline _T*					grid_transpose				( _T (&targetGrid)[_Size][_Size] )									{
		for( uint32_t z=0; z < _Size; ++z )
			for( uint32_t x=z+1; x < _Size; ++x ) {
				_T																				otherValue					=  targetGrid[x][z];
				targetGrid[x][z]															= targetGrid[z][x];
				targetGrid[z][x]															= otherValue;
			}
		return &targetGrid[0][0];																		
	}
}

#endif __VALUE_H__92837423__
