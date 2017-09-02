/// Copyright 2011-2017 - asm128
#include "datatype.h"
#include "nwol_evaluation.h"

#include "nwol_io.h"
#include "nwol_string.h"

//
uint32_t								nwol::getStringFromDataType		( GDATA_TYPE dataType, char* string ) {
	static const char							* typeUINT						= "UINT"
		,										* typeINT						= "INT" 
		,										* typeFLOAT						= "FLOAT"
		,										* typeUFLOAT					= "UFLOAT"
		,										* typeNORM						= "_NORM"	 
		,										* typeUNORM						= ""
		,										* typePAD						= "_P"
		,										* typeNOPAD						= ""
		;
	const char									* typeString						= GTYPEID_ISFLOAT(dataType) ? (GTYPEID_ISSIGNED(dataType)?typeFLOAT:typeUFLOAT) : (GTYPEID_ISSIGNED(dataType)?typeINT:typeUINT);
	char										typeElements	[16]			= {0};
	uint32_t									elementCount					= GTYPEID_ELEMENTCOUNT(dataType);
	if( elementCount > 1 )
		sprintf_s(typeElements, "_%u", elementCount);
			
	char										outString		[64]			= {0};
	sprintf_s(outString, "GDATA_TYPE_%s%u%s%s%s", typeString, GTYPEID_ELEMENTSIZE(dataType), GTYPEID_ISNORMALIZED(dataType) ? typeNORM : typeUNORM, typeElements, GTYPEID_ELEMENTPAD(dataType) ? typePAD : typeNOPAD );
	uint32_t									resultLen						= (uint32_t)strlen(outString);
	if(string)
		strcpy_s(string, resultLen+1, outString);
	return resultLen;
}
