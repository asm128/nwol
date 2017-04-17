#include "nwol_string.h"

#ifndef __NWOL_VALUE_H__92837423__
#define __NWOL_VALUE_H__92837423__

namespace nwol
{
	template <typename _tValue>					static inline const _tValue&		get_value_at_offset			( const void * bytes, uint32_t offset )								{ return *reinterpret_cast<const	_tValue*>(&((const	char*)bytes)[offset]);									}
	template <typename _tValue>					static inline _tValue&				get_value_at_offset			(		void * bytes, uint32_t offset )								{ return *reinterpret_cast<			_tValue*>(&((		char*)bytes)[offset]);									}
	template <typename _tValue>					static inline void					set_value_at_offset			(		void * bytes, uint32_t offset, const	_tValue & value )	{ *reinterpret_cast<_tValue*>(&((char*)bytes)[offset]) = value;													}
	template <typename _tValue>					static inline void					memcpy_value_from_offset	( const void * bytes, uint32_t offset,			_tValue & value )	{ const _tValue* at = (const	_tValue*)&((const	char*)bytes)[offset]; memcpy(&value, at, sizeof(_tValue));	}
	template <typename _tValue>					static inline void					memcpy_value_to_offset		(		void * bytes, uint32_t offset, const	_tValue & value )	{		_tValue* at = (			_tValue*)&((		char*)bytes)[offset]; memcpy(at, &value, sizeof(_tValue));	}

	template <typename _tValue>					static inline uint32_t				get_value_index				( const _tValue& value, const _tValue* arrayValues, uint32_t nMaxCount )		{
		for( uint32_t i=0; i<nMaxCount; i++ )
			if( arrayValues[i] == value )
				return i;

		return -1;
	}
	template <typename _tValue, size_t _Size>	static inline uint32_t				get_value_index				( const _tValue& value, const _tValue (&arrayValues)[_Size] )					{
		for( uint32_t i=0; i<_Size; ++i )
			if( arrayValues[i] == value )
				return i;

		return -1;
	}
	template<typename _tValue, size_t _Size>		static inline _tValue*			array_copy					( _tValue (&targetArray)[_Size], _tValue (&sourceArray)[_Size] )				{																								
		for( uint32_t i=0; i < _Size; ++i	)											
			targetArray[i]																	= sourceArray[i];																		

		return targetArray;																				
	}
	template<typename _tValue, size_t _Size>		static inline _tValue*			grid_copy					( _tValue (&targetGrid)[_Size][_Size], _tValue (&sourceGrid)[_Size][_Size] )	{																								
		for( uint32_t z=0; z < _Size; ++z	)											
			for( uint32_t x=0; x < _Size; ++x	)											
				targetGrid[z][x]																= sourceGrid[z][x];																		

		return &targetGrid[0][0];																				
	}
	template<typename _tValue, size_t _Size>		static inline _tValue*			grid_transpose				( _tValue (&targetGrid)[_Size][_Size], _tValue (&sourceGrid)[_Size][_Size] )	{
		for( uint32_t z=0; z < _Size; z++ )
			for( uint32_t x=0; x < _Size; x++ )
				targetGrid[x][z]															= sourceGrid[z][x];

		return &targetGrid[0][0];																		
	}
	template<typename _tValue, size_t _Size>		static inline _tValue*			grid_transpose				( _tValue (&targetGrid)[_Size][_Size] )											{
		for( uint32_t z=0; z < _Size; ++z )
			for( uint32_t x=z+1; x < _Size; ++x ) {
				_tValue																			otherValue					=  targetGrid[x][z];
				targetGrid[x][z]															= targetGrid[z][x];
				targetGrid[z][x]															= otherValue;
			}
		return &targetGrid[0][0];																		
	}
}

#endif __VALUE_H__92837423__
