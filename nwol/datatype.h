/// This file is part of the Game Object Data System
/// Copyright © 2011-2016: Pablo Ariel Zorrilla Cepeda
///
#include "evaluation.h"
#include "typeint.h"

// --------------------------------------------------------------- GDATA_TYPE ---------------------------------------------------------------
#ifndef __DATATYPE_H__263544625434625344652434321__	
#define __DATATYPE_H__263544625434625344652434321__ 	

namespace nwol
{
#pragma pack(push, 1)
	enum GDATA_TYPE : uint32_t 
		{	GDATA_TYPE_IS_SIGNED			= 0x80000000
		,	GDATA_TYPE_IS_NORMALIZED		= 0x40000000
		,	GDATA_TYPE_IS_FLOAT				= 0x20000000
		,	GDATA_TYPE_IS_BIG_ENDIAN		= 0x10000000
		,	GDATA_TYPE_ELEMENT_COUNT_MASK	= 0x0FFFFF00
		,	GDATA_TYPE_HAS_INNER_PADDING	= 0x80
		,	GDATA_TYPE_BIT_COUNT_MASK		= 0x7F
		};

// Data types (may or may not be -completely?- supported by the hardware)											
// 																													
// This was made to represent data types for a variety of hardware and OSs.											
// This system would allow us to represent type sizes up to 1,048,576 elements of 128 bits (16mib total?).			
// as signed/unsigned integer, signed/unsigned float, signed/unsigned normalized integer,							
// signed/unsigned normalized float, all of them in both little or big endianness									
//																													
// Bits are arranged such that: (right-to-left bit ordering)														
// bits 1 to 7	: element size in bits-1																			
// 8th bit		: 0 = no inner padding for elements / 1 = inner padding for elements to next byte size				
// bits 9 to 28	: element count-1																					
// 29th bit		: 0 = little endian / 1 = big endian																
// 30th bit		: 0 = int / 1 = float																				
// 31th bit		: 0 = Unorm / 1 = Norm																				
// 32th bit		: 0 = Signed / 1 = Unsigned																			
//
#define GTYPEID_MAKE( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )	\
	(::nwol::GDATA_TYPE(																				\
			((IsSigned)			? 0 : ::nwol::GDATA_TYPE_IS_SIGNED)										\
		|	((IsNorm)			? ::nwol::GDATA_TYPE_IS_NORMALIZED		 : 0)							\
		|	((IsFloat)			? ::nwol::GDATA_TYPE_IS_FLOAT			 : 0)							\
		|	((IsBigEndian)		? ::nwol::GDATA_TYPE_IS_BIG_ENDIAN		 : 0)							\
		|	((((ElementCount)-1) & 0xFFFFF) << 8)														\
		|	((ElementPad)		? ::nwol::GDATA_TYPE_HAS_INNER_PADDING	 : 0)							\
		|	(((SizeInBits)-1)	& 0x0000007F	)														\
	))																													

#define	GTYPEID_ISSIGNED(		_dataTypeID )	(0U == ( ::nwol::GDATA_TYPE_IS_SIGNED			& _dataTypeID ))		// returns true if signed
#define	GTYPEID_ISNORMALIZED(	_dataTypeID )	(0U != ( ::nwol::GDATA_TYPE_IS_NORMALIZED		& _dataTypeID ))		// returns true if the value represents a range between 0 and 1
#define	GTYPEID_ISFLOAT(		_dataTypeID )	(0U != ( ::nwol::GDATA_TYPE_IS_FLOAT			& _dataTypeID ))		// returns true if the element bits represent a floating point number
#define	GTYPEID_ISBIGENDIAN(	_dataTypeID )	(0U != ( ::nwol::GDATA_TYPE_IS_BIG_ENDIAN		& _dataTypeID ))		// returns true if the element bits has the bits inverted																				
#define	GTYPEID_ELEMENTPAD(		_dataTypeID )	(0U != ( ::nwol::GDATA_TYPE_HAS_INNER_PADDING	& _dataTypeID ))		// returns true if the elements have inner padding to next byte size																																
#define	GTYPEID_ELEMENTCOUNT(	_dataTypeID )	(((0x0FFFFF00U	& _dataTypeID) >> 8) + 1U)								// returns the element count (up to 1024k elements)																		
#define	GTYPEID_ELEMENTSIZE(	_dataTypeID )	(( 0x7FU		& _dataTypeID) + 1U)									// returns the element size in bits (up to 256 bits)

// Return the byte count of the element type (ElementSize/8). Note: this funcion pads leading bits to next byte size.
#define	GTYPEID_ELEMENTBYTES( _dataTypeID )		((GTYPEID_ELEMENTSIZE( _dataTypeID )/8)	+ one_if(GTYPEID_ELEMENTSIZE( _dataTypeID ) % 8))	

// Return the byte count of the data type (ElementCount*(ElementSize/8))											
#define	GTYPEID_TOTALBYTES( _dataTypeID )																		\
	( GTYPEID_ELEMENTPAD(_dataTypeID) ?																			\
		(GTYPEID_ELEMENTBYTES( _dataTypeID )*GTYPEID_ELEMENTCOUNT( _dataTypeID )) :								\
		(																										\
			( (GTYPEID_ELEMENTSIZE( _dataTypeID )*(uint32_t)GTYPEID_ELEMENTCOUNT( _dataTypeID )) / 8 )			\
			+ one_if( (GTYPEID_ELEMENTSIZE( _dataTypeID )*(uint32_t)GTYPEID_ELEMENTCOUNT( _dataTypeID )) % 8 )	\
		)																										\
	)		

	struct SDataTypeID {
							uint32_t					SizeInBits		:  7	; 
							uint32_t					ElementPad		:  1	; 
							uint32_t					ElementCount	: 20	; 
							uint32_t					IsBigEndian		:  1	; 
							uint32_t					IsFloat			:  1	; 
							uint32_t					IsNorm			:  1	; 
							uint32_t					IsSigned		:  1	; 

		inline constexpr	uint32_t					ElementBytes	()						const	{ return ( SizeInBits / 8 + one_if( SizeInBits % 8 ) ); }
		inline constexpr	uint32_t					TotalBytes		()						const	{ return ElementPad ? ElementBytes() * ElementCount : SizeInBits * ElementCount / 8 + one_if( SizeInBits * ElementCount % 8 ); }

		inline constexpr	bool						IsSTL			()						const	{ return (1 == SizeInBits) && IsSigned && IsFloat				&& IsNorm				&& (IsBigEndian == false); }
		inline constexpr	bool						IsNonUniform	()						const	{ return (1 == SizeInBits) && IsSigned && (IsFloat == false)	&& (IsNorm == false)	&& (IsBigEndian == false); }
		inline constexpr	bool						IsUniform		()						const	{ return !IsNonUniform(); }

		inline constexpr								SDataTypeID		( GDATA_TYPE typeId )
			: SizeInBits	(GTYPEID_ELEMENTSIZE	(typeId))
			, ElementPad	(GTYPEID_ELEMENTPAD		(typeId))
			, ElementCount	(GTYPEID_ELEMENTCOUNT	(typeId))
			, IsBigEndian	(GTYPEID_ISBIGENDIAN	(typeId))	
			, IsFloat		(GTYPEID_ISFLOAT		(typeId))	
			, IsNorm		(GTYPEID_ISNORMALIZED	(typeId))
			, IsSigned		(GTYPEID_ISSIGNED		(typeId))
		{}

		//
		inline constexpr								SDataTypeID
			( uint8_t	sizeInBits		= 1
			, uint32_t	elementCount	= 1
			, bool		isFloat			= false
			, bool		isSigned		= true
			, bool		isNorm			= false
			, bool		elementPad		= false
			, bool		isBigEndian		= false
			)
			: SizeInBits	(sizeInBits		-1)
			, ElementPad	(elementPad		? 1 : 0)
			, ElementCount	(elementCount	-1)
			, IsBigEndian	(isBigEndian	? 1 : 0)	
			, IsFloat		(isFloat		? 1 : 0)	
			, IsNorm		(isNorm			? 1 : 0)
			, IsSigned		(isSigned		? 0 : 1)
		{}

		inline constexpr	operator					GDATA_TYPE		()							const		{
			return GTYPEID_MAKE
				( !IsSigned			
				, IsNorm				
				, IsFloat		
				, IsBigEndian			
				, ElementCount+1
				, ElementPad		
				, SizeInBits+1
				);
		}

		inline constexpr	bool						operator==		(const GDATA_TYPE& other)	const		{
			return	SizeInBits		==			(GTYPEID_ELEMENTSIZE	(other))
				&&	ElementPad		==			(GTYPEID_ELEMENTPAD		(other) ? 1U : 0U)
				&&	ElementCount	== (uint32_t)GTYPEID_ELEMENTCOUNT	(other)
				&&	IsBigEndian		==			(GTYPEID_ISBIGENDIAN	(other) ? 1U : 0U)
				&&	IsFloat			==			(GTYPEID_ISFLOAT		(other) ? 1U : 0U)
				&&	IsNorm			==			(GTYPEID_ISNORMALIZED	(other) ? 1U : 0U)
				&&	IsSigned		==			(GTYPEID_ISSIGNED		(other) ? 0U : 1U)
				;
		}

		inline constexpr	bool						operator==		(const SDataTypeID& other)	const		{
			return	SizeInBits		==	other.SizeInBits		
				&&	ElementPad		==	other.ElementPad		
				&&	ElementCount	==	other.ElementCount	
				&&	IsBigEndian		==	other.IsBigEndian		
				&&	IsFloat			==	other.IsFloat			
				&&	IsNorm			==	other.IsNorm			
				&&	IsSigned		==	other.IsSigned		
				;
		}

	};
	//----------------------------------------------------------------------------------------------------------------------------------

// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_POINTER_MAKE( TypeName )		::nwol::SDataTypeID((uint8_t)(sizeof(TypeName*)*8)	, 1	, false, false)	// defines an unsigned unorm little endian integer of sizeof(TypeName*) bytes.
#define GTYPEID_DATA_MAKE( TypeName )			::nwol::SDataTypeID(8, sizeof(TypeName)					, false, false)	// defines an array of unsigned unorm little endian sizeof(TypeName) bytes.										

#define GTYPEID_MAKE_NON_UNIFORM(_TypeCode)		::nwol::SDataTypeID(1, _TypeCode, false, true, false)
#define GTYPEID_MAKE_STL(_TypeCode)				::nwol::SDataTypeID(1, _TypeCode, true , true, true )


// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_MAKE_INT(			ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount				)
#define GTYPEID_MAKE_UINT(			ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, false, false	)
#define GTYPEID_MAKE_FLOAT(			ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, true			)
#define GTYPEID_MAKE_UFLOAT(		ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, true , false	)
//
#define GTYPEID_MAKE_INT_NORM(		ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, false, true	, true)
#define GTYPEID_MAKE_UINT_NORM(		ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, false, false	, true)
#define GTYPEID_MAKE_FLOAT_NORM(	ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, true	, true	, true)
#define GTYPEID_MAKE_UFLOAT_NORM(	ElementBits, ElementCount )		::nwol::SDataTypeID(ElementBits, ElementCount, true , false	, true)

// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_MAKE_UINT8(		ElementCount)	GTYPEID_MAKE_UINT	( 8  , ElementCount )
#define GTYPEID_MAKE_UINT16(	ElementCount)	GTYPEID_MAKE_UINT	( 16 , ElementCount )
#define GTYPEID_MAKE_UINT32(	ElementCount)	GTYPEID_MAKE_UINT	( 32 , ElementCount )
#define GTYPEID_MAKE_UINT64(	ElementCount)	GTYPEID_MAKE_UINT	( 64 , ElementCount )
#define GTYPEID_MAKE_INT8(		ElementCount)	GTYPEID_MAKE_INT	( 8  , ElementCount )
#define GTYPEID_MAKE_INT16(		ElementCount)	GTYPEID_MAKE_INT	( 16 , ElementCount )
#define GTYPEID_MAKE_INT32(		ElementCount)	GTYPEID_MAKE_INT	( 32 , ElementCount )
#define GTYPEID_MAKE_INT64(		ElementCount)	GTYPEID_MAKE_INT	( 64 , ElementCount )

#define GTYPEID_MAKE_UFLOAT16(ElementCount)		GTYPEID_MAKE_UFLOAT		( 16 , ElementCount )
#define GTYPEID_MAKE_UFLOAT32(ElementCount)		GTYPEID_MAKE_UFLOAT		( 32 , ElementCount )
#define GTYPEID_MAKE_UFLOAT64(ElementCount)		GTYPEID_MAKE_UFLOAT		( 64 , ElementCount )
#define GTYPEID_MAKE_FLOAT16( ElementCount)		GTYPEID_MAKE_FLOAT		( 16 , ElementCount )
#define GTYPEID_MAKE_FLOAT32( ElementCount)		GTYPEID_MAKE_FLOAT		( 32 , ElementCount )
#define GTYPEID_MAKE_FLOAT64( ElementCount)		GTYPEID_MAKE_FLOAT		( 64 , ElementCount )
	
	// Type ID can now be built with the GTYPEID_MAKE macros, so these would be "helper" constants
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT1				= GTYPEID_MAKE_UINT(1,0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT5				= GTYPEID_MAKE_UINT(5,0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT5_3				= GTYPEID_MAKE_UINT(5,0x003);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8				= GTYPEID_MAKE_UINT8 (0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_2				= GTYPEID_MAKE_UINT8 (0x002);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_3				= GTYPEID_MAKE_UINT8 (0x003);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_4				= GTYPEID_MAKE_UINT8 (0x004);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_5				= GTYPEID_MAKE_UINT8 (0x005);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_6				= GTYPEID_MAKE_UINT8 (0x006);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_7				= GTYPEID_MAKE_UINT8 (0x007);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_8				= GTYPEID_MAKE_UINT8 (0x008);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_9				= GTYPEID_MAKE_UINT8 (0x009);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_16				= GTYPEID_MAKE_UINT8 (0x010);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_32				= GTYPEID_MAKE_UINT8 (0x020);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_64				= GTYPEID_MAKE_UINT8 (0x040);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_128			= GTYPEID_MAKE_UINT8 (0x080);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT8_256			= GTYPEID_MAKE_UINT8 (0x100);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16				= GTYPEID_MAKE_UINT16(0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_2				= GTYPEID_MAKE_UINT16(0x002);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_3				= GTYPEID_MAKE_UINT16(0x003);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_4				= GTYPEID_MAKE_UINT16(0x004);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_5				= GTYPEID_MAKE_UINT16(0x005);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_6				= GTYPEID_MAKE_UINT16(0x006);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_7				= GTYPEID_MAKE_UINT16(0x007);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_8				= GTYPEID_MAKE_UINT16(0x008);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_9				= GTYPEID_MAKE_UINT16(0x009);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_16			= GTYPEID_MAKE_UINT16(0x010);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_32			= GTYPEID_MAKE_UINT16(0x020);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_64			= GTYPEID_MAKE_UINT16(0x040);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_128			= GTYPEID_MAKE_UINT16(0x080);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT16_256			= GTYPEID_MAKE_UINT16(0x100);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32				= GTYPEID_MAKE_UINT32(0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_2				= GTYPEID_MAKE_UINT32(0x002);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_3				= GTYPEID_MAKE_UINT32(0x003);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_4				= GTYPEID_MAKE_UINT32(0x004);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_5				= GTYPEID_MAKE_UINT32(0x005);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_6				= GTYPEID_MAKE_UINT32(0x006);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_7				= GTYPEID_MAKE_UINT32(0x007);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_8				= GTYPEID_MAKE_UINT32(0x008);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_9				= GTYPEID_MAKE_UINT32(0x009);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_16			= GTYPEID_MAKE_UINT32(0x010);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_32			= GTYPEID_MAKE_UINT32(0x020);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_64			= GTYPEID_MAKE_UINT32(0x040);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_128			= GTYPEID_MAKE_UINT32(0x080);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT32_256			= GTYPEID_MAKE_UINT32(0x100);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64				= GTYPEID_MAKE_UINT64(0x001);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_2				= GTYPEID_MAKE_UINT64(0x002);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_3				= GTYPEID_MAKE_UINT64(0x003);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_4				= GTYPEID_MAKE_UINT64(0x004);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_5				= GTYPEID_MAKE_UINT64(0x005);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_6				= GTYPEID_MAKE_UINT64(0x006);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_7				= GTYPEID_MAKE_UINT64(0x007);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_8				= GTYPEID_MAKE_UINT64(0x008);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_9				= GTYPEID_MAKE_UINT64(0x009);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_16			= GTYPEID_MAKE_UINT64(0x010);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_32			= GTYPEID_MAKE_UINT64(0x020);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_64			= GTYPEID_MAKE_UINT64(0x040);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_128			= GTYPEID_MAKE_UINT64(0x080);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_UINT64_256			= GTYPEID_MAKE_UINT64(0x100);	
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT5					= GTYPEID_MAKE_INT(5,0x001);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT5_3				= GTYPEID_MAKE_INT(5,0x003);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT8					= GTYPEID_MAKE_INT8 (0x001);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT8_2				= GTYPEID_MAKE_INT8 (0x002);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT8_3				= GTYPEID_MAKE_INT8 (0x003);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT8_4				= GTYPEID_MAKE_INT8 (0x004);		
	static constexpr		const GDATA_TYPE			GDATA_TYPE_INT8_5				= GTYPEID_MAKE_INT8 (0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_6				= GTYPEID_MAKE_INT8 (0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_7				= GTYPEID_MAKE_INT8 (0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_8				= GTYPEID_MAKE_INT8 (0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_9				= GTYPEID_MAKE_INT8 (0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_16				= GTYPEID_MAKE_INT8 (0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_32				= GTYPEID_MAKE_INT8 (0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_64				= GTYPEID_MAKE_INT8 (0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_128				= GTYPEID_MAKE_INT8 (0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_256				= GTYPEID_MAKE_INT8 (0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16				= GTYPEID_MAKE_INT16(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_2				= GTYPEID_MAKE_INT16(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_3				= GTYPEID_MAKE_INT16(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_4				= GTYPEID_MAKE_INT16(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_5				= GTYPEID_MAKE_INT16(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_6				= GTYPEID_MAKE_INT16(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_7				= GTYPEID_MAKE_INT16(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_8				= GTYPEID_MAKE_INT16(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_9				= GTYPEID_MAKE_INT16(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_16				= GTYPEID_MAKE_INT16(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_32				= GTYPEID_MAKE_INT16(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_64				= GTYPEID_MAKE_INT16(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_128			= GTYPEID_MAKE_INT16(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_256			= GTYPEID_MAKE_INT16(0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32				= GTYPEID_MAKE_INT32(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_2				= GTYPEID_MAKE_INT32(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_3				= GTYPEID_MAKE_INT32(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_4				= GTYPEID_MAKE_INT32(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_5				= GTYPEID_MAKE_INT32(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_6				= GTYPEID_MAKE_INT32(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_7				= GTYPEID_MAKE_INT32(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_8				= GTYPEID_MAKE_INT32(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_9				= GTYPEID_MAKE_INT32(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_16				= GTYPEID_MAKE_INT32(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_32				= GTYPEID_MAKE_INT32(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_64				= GTYPEID_MAKE_INT32(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_128			= GTYPEID_MAKE_INT32(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_256			= GTYPEID_MAKE_INT32(0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64				= GTYPEID_MAKE_INT64(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_2				= GTYPEID_MAKE_INT64(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_3				= GTYPEID_MAKE_INT64(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_4				= GTYPEID_MAKE_INT64(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_5				= GTYPEID_MAKE_INT64(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_6				= GTYPEID_MAKE_INT64(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_7				= GTYPEID_MAKE_INT64(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_8				= GTYPEID_MAKE_INT64(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_9				= GTYPEID_MAKE_INT64(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_16				= GTYPEID_MAKE_INT64(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_32				= GTYPEID_MAKE_INT64(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_64				= GTYPEID_MAKE_INT64(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_128			= GTYPEID_MAKE_INT64(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_256			= GTYPEID_MAKE_INT64(0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_256		= GTYPEID_MAKE_FLOAT16(0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16				= GTYPEID_MAKE_FLOAT16(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_2			= GTYPEID_MAKE_FLOAT16(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_3			= GTYPEID_MAKE_FLOAT16(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_4			= GTYPEID_MAKE_FLOAT16(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_5			= GTYPEID_MAKE_FLOAT16(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_6			= GTYPEID_MAKE_FLOAT16(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_7			= GTYPEID_MAKE_FLOAT16(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_8			= GTYPEID_MAKE_FLOAT16(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_9			= GTYPEID_MAKE_FLOAT16(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_16			= GTYPEID_MAKE_FLOAT16(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_32			= GTYPEID_MAKE_FLOAT16(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_64			= GTYPEID_MAKE_FLOAT16(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_128			= GTYPEID_MAKE_FLOAT16(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_256			= GTYPEID_MAKE_FLOAT16(0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32				= GTYPEID_MAKE_FLOAT32(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_2			= GTYPEID_MAKE_FLOAT32(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_3			= GTYPEID_MAKE_FLOAT32(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_4			= GTYPEID_MAKE_FLOAT32(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_5			= GTYPEID_MAKE_FLOAT32(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_6			= GTYPEID_MAKE_FLOAT32(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_7			= GTYPEID_MAKE_FLOAT32(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_8			= GTYPEID_MAKE_FLOAT32(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_9			= GTYPEID_MAKE_FLOAT32(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_16			= GTYPEID_MAKE_FLOAT32(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_32			= GTYPEID_MAKE_FLOAT32(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_64			= GTYPEID_MAKE_FLOAT32(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_128			= GTYPEID_MAKE_FLOAT32(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_256			= GTYPEID_MAKE_FLOAT32(0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64				= GTYPEID_MAKE_FLOAT64(0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_2			= GTYPEID_MAKE_FLOAT64(0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_3			= GTYPEID_MAKE_FLOAT64(0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_4			= GTYPEID_MAKE_FLOAT64(0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_5			= GTYPEID_MAKE_FLOAT64(0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_6			= GTYPEID_MAKE_FLOAT64(0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_7			= GTYPEID_MAKE_FLOAT64(0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_8			= GTYPEID_MAKE_FLOAT64(0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_9			= GTYPEID_MAKE_FLOAT64(0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_16			= GTYPEID_MAKE_FLOAT64(0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_32			= GTYPEID_MAKE_FLOAT64(0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_64			= GTYPEID_MAKE_FLOAT64(0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_128			= GTYPEID_MAKE_FLOAT64(0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_256			= GTYPEID_MAKE_FLOAT64(0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT5_NORM			= GTYPEID_MAKE_UINT_NORM(0x05,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT5_NORM_3			= GTYPEID_MAKE_UINT_NORM(0x05,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM			= GTYPEID_MAKE_UINT_NORM(0x08,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_2			= GTYPEID_MAKE_UINT_NORM(0x08,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_3			= GTYPEID_MAKE_UINT_NORM(0x08,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_4			= GTYPEID_MAKE_UINT_NORM(0x08,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_5			= GTYPEID_MAKE_UINT_NORM(0x08,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_6			= GTYPEID_MAKE_UINT_NORM(0x08,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_7			= GTYPEID_MAKE_UINT_NORM(0x08,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_8			= GTYPEID_MAKE_UINT_NORM(0x08,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_9			= GTYPEID_MAKE_UINT_NORM(0x08,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_16		= GTYPEID_MAKE_UINT_NORM(0x08,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_32		= GTYPEID_MAKE_UINT_NORM(0x08,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_64		= GTYPEID_MAKE_UINT_NORM(0x08,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_128		= GTYPEID_MAKE_UINT_NORM(0x08,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT8_NORM_256		= GTYPEID_MAKE_UINT_NORM(0x08,0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM			= GTYPEID_MAKE_UINT_NORM(0x10,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_2		= GTYPEID_MAKE_UINT_NORM(0x10,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_3		= GTYPEID_MAKE_UINT_NORM(0x10,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_4		= GTYPEID_MAKE_UINT_NORM(0x10,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_5		= GTYPEID_MAKE_UINT_NORM(0x10,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_6		= GTYPEID_MAKE_UINT_NORM(0x10,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_7		= GTYPEID_MAKE_UINT_NORM(0x10,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_8		= GTYPEID_MAKE_UINT_NORM(0x10,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_9		= GTYPEID_MAKE_UINT_NORM(0x10,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_16		= GTYPEID_MAKE_UINT_NORM(0x10,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_32		= GTYPEID_MAKE_UINT_NORM(0x10,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_64		= GTYPEID_MAKE_UINT_NORM(0x10,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_128		= GTYPEID_MAKE_UINT_NORM(0x10,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT16_NORM_256		= GTYPEID_MAKE_UINT_NORM(0x10,0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM			= GTYPEID_MAKE_UINT_NORM(0x20,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_2		= GTYPEID_MAKE_UINT_NORM(0x20,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_3		= GTYPEID_MAKE_UINT_NORM(0x20,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_4		= GTYPEID_MAKE_UINT_NORM(0x20,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_5		= GTYPEID_MAKE_UINT_NORM(0x20,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_6		= GTYPEID_MAKE_UINT_NORM(0x20,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_7		= GTYPEID_MAKE_UINT_NORM(0x20,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_8		= GTYPEID_MAKE_UINT_NORM(0x20,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_9		= GTYPEID_MAKE_UINT_NORM(0x20,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_16		= GTYPEID_MAKE_UINT_NORM(0x20,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_32		= GTYPEID_MAKE_UINT_NORM(0x20,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_64		= GTYPEID_MAKE_UINT_NORM(0x20,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_128		= GTYPEID_MAKE_UINT_NORM(0x20,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT32_NORM_256		= GTYPEID_MAKE_UINT_NORM(0x20,0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM			= GTYPEID_MAKE_UINT_NORM(0x40,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_2		= GTYPEID_MAKE_UINT_NORM(0x40,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_3		= GTYPEID_MAKE_UINT_NORM(0x40,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_4		= GTYPEID_MAKE_UINT_NORM(0x40,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_5		= GTYPEID_MAKE_UINT_NORM(0x40,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_6		= GTYPEID_MAKE_UINT_NORM(0x40,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_7		= GTYPEID_MAKE_UINT_NORM(0x40,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_8		= GTYPEID_MAKE_UINT_NORM(0x40,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_9		= GTYPEID_MAKE_UINT_NORM(0x40,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_16		= GTYPEID_MAKE_UINT_NORM(0x40,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_32		= GTYPEID_MAKE_UINT_NORM(0x40,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_64		= GTYPEID_MAKE_UINT_NORM(0x40,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_128		= GTYPEID_MAKE_UINT_NORM(0x40,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_UINT64_NORM_256		= GTYPEID_MAKE_UINT_NORM(0x40,0x100);	
	static constexpr		const SDataTypeID			GDATA_TYPE_INT5_NORM			= GTYPEID_MAKE_INT_NORM (0x05,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT5_NORM_3			= GTYPEID_MAKE_INT_NORM (0x05,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM			= GTYPEID_MAKE_INT_NORM (0x08,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_2			= GTYPEID_MAKE_INT_NORM (0x08,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_3			= GTYPEID_MAKE_INT_NORM (0x08,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_4			= GTYPEID_MAKE_INT_NORM (0x08,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_5			= GTYPEID_MAKE_INT_NORM (0x08,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_6			= GTYPEID_MAKE_INT_NORM (0x08,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_7			= GTYPEID_MAKE_INT_NORM (0x08,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_8			= GTYPEID_MAKE_INT_NORM (0x08,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_9			= GTYPEID_MAKE_INT_NORM (0x08,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_16			= GTYPEID_MAKE_INT_NORM (0x08,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_32			= GTYPEID_MAKE_INT_NORM (0x08,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_64			= GTYPEID_MAKE_INT_NORM (0x08,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_128		= GTYPEID_MAKE_INT_NORM (0x08,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT8_NORM_256		= GTYPEID_MAKE_INT_NORM (0x08,0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM			= GTYPEID_MAKE_INT_NORM (0x10,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_2			= GTYPEID_MAKE_INT_NORM (0x10,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_3			= GTYPEID_MAKE_INT_NORM (0x10,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_4			= GTYPEID_MAKE_INT_NORM (0x10,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_5			= GTYPEID_MAKE_INT_NORM (0x10,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_6			= GTYPEID_MAKE_INT_NORM (0x10,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_7			= GTYPEID_MAKE_INT_NORM (0x10,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_8			= GTYPEID_MAKE_INT_NORM (0x10,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_9			= GTYPEID_MAKE_INT_NORM (0x10,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_16		= GTYPEID_MAKE_INT_NORM (0x10,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_32		= GTYPEID_MAKE_INT_NORM (0x10,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_64		= GTYPEID_MAKE_INT_NORM (0x10,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_128		= GTYPEID_MAKE_INT_NORM (0x10,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT16_NORM_256		= GTYPEID_MAKE_INT_NORM (0x10,0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM			= GTYPEID_MAKE_INT_NORM (0x20,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_2			= GTYPEID_MAKE_INT_NORM (0x20,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_3			= GTYPEID_MAKE_INT_NORM (0x20,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_4			= GTYPEID_MAKE_INT_NORM (0x20,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_5			= GTYPEID_MAKE_INT_NORM (0x20,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_6			= GTYPEID_MAKE_INT_NORM (0x20,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_7			= GTYPEID_MAKE_INT_NORM (0x20,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_8			= GTYPEID_MAKE_INT_NORM (0x20,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_9			= GTYPEID_MAKE_INT_NORM (0x20,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_16		= GTYPEID_MAKE_INT_NORM (0x20,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_32		= GTYPEID_MAKE_INT_NORM (0x20,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_64		= GTYPEID_MAKE_INT_NORM (0x20,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_128		= GTYPEID_MAKE_INT_NORM (0x20,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT32_NORM_256		= GTYPEID_MAKE_INT_NORM (0x20,0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM			= GTYPEID_MAKE_INT_NORM (0x40,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_2			= GTYPEID_MAKE_INT_NORM (0x40,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_3			= GTYPEID_MAKE_INT_NORM (0x40,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_4			= GTYPEID_MAKE_INT_NORM (0x40,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_5			= GTYPEID_MAKE_INT_NORM (0x40,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_6			= GTYPEID_MAKE_INT_NORM (0x40,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_7			= GTYPEID_MAKE_INT_NORM (0x40,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_8			= GTYPEID_MAKE_INT_NORM (0x40,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_9			= GTYPEID_MAKE_INT_NORM (0x40,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_16		= GTYPEID_MAKE_INT_NORM (0x40,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_32		= GTYPEID_MAKE_INT_NORM (0x40,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_64		= GTYPEID_MAKE_INT_NORM (0x40,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_INT64_NORM_128		= GTYPEID_MAKE_INT_NORM (0x40,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM			= GTYPEID_MAKE_FLOAT_NORM(0x10,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_2		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_3		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_4		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_5		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_6		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_7		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_8		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_9		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_16		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_32		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_64		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_128		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT16_NORM_256		= GTYPEID_MAKE_FLOAT_NORM(0x10,0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM			= GTYPEID_MAKE_FLOAT_NORM(0x20,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_2		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_3		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_4		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_5		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_6		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_7		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_8		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_9		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_16		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_32		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_64		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_128		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT32_NORM_256		= GTYPEID_MAKE_FLOAT_NORM(0x20,0x100);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM			= GTYPEID_MAKE_FLOAT_NORM(0x40,0x001);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_2		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x002);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_3		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x003);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_4		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x004);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_5		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x005);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_6		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x006);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_7		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x007);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_8		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x008);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_9		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x009);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_16		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x010);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_32		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x020);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_64		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x040);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_128		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x080);		
	static constexpr		const SDataTypeID			GDATA_TYPE_FLOAT64_NORM_256		= GTYPEID_MAKE_FLOAT_NORM(0x40,0x100);		
	
	// --------------		------------------------	------------------------- GDATA_TYPE ---------------------------------------------------------------
	// -- Non-POD typ		es:
	static constexpr		const SDataTypeID			GDATA_TYPE_UNKNOWN				= GTYPEID_MAKE_NON_UNIFORM(0x01);	// aka GDATA_TYPE_INT1 
	static constexpr		const SDataTypeID			GDATA_TYPE_NWOLVECTOR			= GTYPEID_MAKE_NON_UNIFORM(0x02);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_NWOLSTRING			= GTYPEID_MAKE_NON_UNIFORM(0x03);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GLABEL				= GTYPEID_MAKE_NON_UNIFORM(0x04);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GENUM				= GTYPEID_MAKE_NON_UNIFORM(0x05);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GDESCRIPTOR			= GTYPEID_MAKE_NON_UNIFORM(0x06);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GPOD					= GTYPEID_MAKE_NON_UNIFORM(0x07);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GOBJ					= GTYPEID_MAKE_NON_UNIFORM(0x08);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GNCO					= GTYPEID_MAKE_NON_UNIFORM(0x09);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GREF					= GTYPEID_MAKE_NON_UNIFORM(0x0A);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GLIST				= GTYPEID_MAKE_NON_UNIFORM(0x0B);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_GPURE				= GTYPEID_MAKE_NON_UNIFORM(0x0C);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_CUSTOM				= GTYPEID_MAKE_NON_UNIFORM(0x0D);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_POD					= GTYPEID_MAKE_NON_UNIFORM(0x0E);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_OBJ					= GTYPEID_MAKE_NON_UNIFORM(0x0F);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_NCO					= GTYPEID_MAKE_NON_UNIFORM(0x10);	// 
	static constexpr		const SDataTypeID			GDATA_TYPE_MOD					= GTYPEID_MAKE_NON_UNIFORM(0x11);	//
	static constexpr		const SDataTypeID			GDATA_TYPE_FUN					= GTYPEID_MAKE_NON_UNIFORM(0x12);	//
	static constexpr		const SDataTypeID			GDATA_TYPE_POINTER				= GTYPEID_MAKE_NON_UNIFORM(sizeof(void*)*8);	// notice that this definition changes depending on the platform

	// -- STL types
	static constexpr		const SDataTypeID			GDATA_TYPE_STLOTHER				= GTYPEID_MAKE_STL(0x01);	// aka GDATA_TYPE_INT1_14
	static constexpr		const SDataTypeID			GDATA_TYPE_STLVECTOR			= GTYPEID_MAKE_STL(0x02);	// aka GDATA_TYPE_INT1_14
	static constexpr		const SDataTypeID			GDATA_TYPE_STLSTRING			= GTYPEID_MAKE_STL(0x03);	// aka GDATA_TYPE_INT1_15
	
	// -- Random bullshit that can cause a lot of problems such as security vulnerabilities, buffer overflows, hidden bugs, portability problems, increasing code size, etc.
	static constexpr		const SDataTypeID			GDATA_TYPE_SIZE_T				= ::nwol::SDataTypeID(sizeof(size_t)*8, 1, false, sizeof(size_t) == 4 ? false : true);

	//}; // GDATA_TYPE
	// ------------------------------------------ Aliases
#define GDATA_TYPE_DATATYPE	GDATA_TYPE_UINT32	
#define GDATA_TYPE_BOOL		GDATA_TYPE_UINT8	

#pragma pack(pop)

	uint32_t	getStringFromDataType( GDATA_TYPE dataType, char* string );	

#if defined(__WINDOWS__)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

} // namespace 

#endif // __DATATYPE_H__263544625434625344652434321__
