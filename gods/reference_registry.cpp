#include "reference_registry.h"
#include "gref_definition.h"
#include "stype.h"

GDEFINE_OBJ(nwol, CReferenceContainer);

uint32_t										nwol::CReferenceContainer::Save					( char* out_pMemoryBuffer )			const		{
	uint32_t											totalBytes				= 0;
	::nwol::suint32										dataCount				= DataNames.size();

	totalBytes										+= dataCount.write(out_pMemoryBuffer);
	for(uint32_t i=0; i<dataCount; i++)	totalBytes		+= DataNames		[i].save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);
	for(uint32_t i=0; i<dataCount; i++)	totalBytes		+= DataDescriptions	[i].save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);

	return totalBytes;
}

uint32_t										nwol::CReferenceContainer::Load					( const char* in_pMemoryBuffer )				{
	uint32_t											totalBytes				= 0;
	::nwol::suint32										dataCount				= 0;

	totalBytes										+= dataCount.read(in_pMemoryBuffer);
	DataNames			.resize(dataCount);
	DataDescriptions	.resize(dataCount);
	
	for(uint32_t i=0; i<dataCount; i++)	totalBytes		+= DataNames		.begin()[i].load(&in_pMemoryBuffer[totalBytes]);
	for(uint32_t i=0; i<dataCount; i++)	totalBytes		+= DataDescriptions	.begin()[i].load(&in_pMemoryBuffer[totalBytes]);

	release_all();	// Release all references before resizing.
	
	Data	.resize(dataCount);
	memset(Data.begin(), 0, sizeof(void*)*dataCount);	// Clear out pointers.

	return totalBytes;
}
