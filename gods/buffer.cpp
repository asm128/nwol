/// This file is part of the Game Object Data System
/// Copyright © 2011: Pablo Ariel Zorrilla Cepeda
#include "buffer.h"
#include "gref_definition.h"
#include "gref_ptr.h"

GDEFINE_PRINTPOD( nwol, SBuffer,
	"{'DataFormat': 0x%X,\n"
	" 'Usage': 0x%X,\n"
	" 'nElementCount': %u,\n"
	" 'nColumnCount': %u,\n"
	" 'nSliceCount': %u,\n"
	" 'nSizeInBytes': %u,\n"
	" 'pByteArray': 0x%p}"
	" '__pBlock': 0x%p}"
	, (unsigned int)Data->DataFormat
	, (unsigned int)Data->Usage
	, (unsigned int)Data->nElementCount
	, (unsigned int)Data->nColumnCount
	, (unsigned int)Data->nSliceCount
	, (unsigned int)Data->nSizeInBytes
	, Data->pByteArray
	, Data->__pBlock
	);

// the following functions discards comparing the void* address if all other data and void* contents match. 
int32_t										nwol::gcompare											(const GREF(SBuffer)* BufferA, const GREF(SBuffer)* BufferB) {
	if (BufferA == BufferB)		// If pointers are the same, return equal
		return 0;
	if (!BufferA || !BufferB)	// If one of the pointers is null, return not equal (as buffers are supposed to have always a size in bytes)
		return -1;
	const ::nwol::SBuffer								* bA												= BufferA->get()
		,												* bB												= BufferB->get()
		;
	int													check												= //memcmp( _startA, _startB, sizeof(Buffer)-sizeof( void* ) );	// compare everything from there except for the void*
		  ((bA->DataFormat		== bB->DataFormat)
		&& (bA->nElementCount	== bB->nElementCount)
		&& (bA->nColumnCount	== bB->nColumnCount)
		&& (bA->nSliceCount		== bB->nSliceCount)
		&& (bA->Usage			== bB->Usage)
		) ? 0 : -1;
	if (check)	// if the data differs, return not equal
		return -1;
	if (0 != (check = memcmp(bA->pByteArray, bB->pByteArray, bA->nElementCount*GTYPEID_TOTALBYTES(bA->DataFormat)))) // test only for the valid bytes and not the array size
		return check;	// if the data contents differ, return equal.
	return 0;
}

// the following functions discards comparing the void* address if all other data and void* contents match. 
int32_t										nwol::gcompare											(const SBuffer* bA, const SBuffer* bB) {
	if (bA == bB)		// If pointers are the same, return equal
		return 0;
	if (!bA || !bB)	// If one of the pointers is null, return not equal (as buffers are supposed to have always a size in bytes)
		return -1;
	
	int													check												= 
		(	(bA->DataFormat		== bB->DataFormat)
		&&	(bA->nElementCount	== bB->nElementCount)
		&&	(bA->nColumnCount	== bB->nColumnCount)
		&&	(bA->nSliceCount	== bB->nSliceCount)
		&&	(bA->Usage			== bB->Usage)
		) ? 0 : -1;

	if (check)	// if the data differs, return not equal
		return -1;
	
	if (0 != (check = memcmp(bA->pByteArray, bB->pByteArray, bA->nElementCount*GTYPEID_TOTALBYTES(bA->DataFormat)))) // test only for the valid bytes and not the array size
		return check;	// if the data contents differ, return equal.
	return 0;
}

::nwol::error_t								nwol::cloneBuffer											(GODS(SBuffer)* ppdstData, const GREF(SBuffer)* psrcData) {
	if (0 == psrcData) {
		::nwol::release(ppdstData);
		return 0;
	}
	GPNCO(::nwol, SBuffer)								newData, oldData;
	const ::nwol::SBuffer								* srcInstance										= psrcData->get();
	nwol_necall(::nwol::createBuffer(srcInstance->DataFormat, srcInstance->Usage, srcInstance->nElementCount, srcInstance->nColumnCount, srcInstance->nSliceCount, &newData), "createBuffer() failed for %u elements. Out of memory?", srcInstance->nElementCount);

	memcpy(newData->pByteArray, srcInstance->pByteArray, GTYPEID_TOTALBYTES(newData->DataFormat)*newData->nElementCount);
	oldData											= *ppdstData;
	(*ppdstData)									= newData.acquire();
	return 0;
}

::nwol::error_t								nwol::joinBuffers										(const GODS(SBuffer) in_BufferA, const GODS(SBuffer) in_BufferB, ::nwol::GODS(SBuffer)* out_JoinedBuffer) {
	GPNCO(::nwol, SBuffer)								joinedBuffer;
	bool												bReturn												= false;
		 if (0 == in_BufferB && 0 == in_BufferA)	{ ::nwol::release(out_JoinedBuffer);			bReturn		= true; }
	else if (0 == in_BufferB)						{ ::nwol::set(out_JoinedBuffer, in_BufferA);	bReturn		= true; }
	else if (0 == in_BufferA)						{ ::nwol::set(out_JoinedBuffer, in_BufferB);	bReturn		= true; }

	if( bReturn )
		return 0;

	const ::nwol::SBuffer								* bA												= in_BufferA->get()
		,												* bB												= in_BufferB->get()
		;

	uint32_t											newBufferSize										= bA->nElementCount + bB->nElementCount;
	nwol_necall(::nwol::createBuffer(bA->DataFormat, bA->Usage, newBufferSize, &joinedBuffer), "createBuffer() failed for %u elements. Out of memory?", newBufferSize);

	uint32_t											typeSize											= GTYPEID_TOTALBYTES(bA->DataFormat);
	uint32_t											nCurrentBufferSizeInBytes							= bA->nElementCount*typeSize
		,												nOtherBufferSizeInBytes								= bB->nElementCount*typeSize
		;
	memcpy(joinedBuffer->pByteArray, bA->pByteArray, nCurrentBufferSizeInBytes);
	memcpy(&((char*)joinedBuffer->pByteArray)[nCurrentBufferSizeInBytes], bB->pByteArray, nOtherBufferSizeInBytes);

	if (joinedBuffer->Usage == GUSAGE_TEXT) {
		uint32_t											elemEnd												= nCurrentBufferSizeInBytes + nOtherBufferSizeInBytes;
		memset(&((char*)joinedBuffer->pByteArray)[elemEnd], 0, typeSize);
	}

	if (out_JoinedBuffer)
		::nwol::set(out_JoinedBuffer, joinedBuffer.get_pointer());
	return 0;
}

::nwol::error_t								nwol::splitBuffer										(const GODS(SBuffer) in_BufferToSplit, uint32_t nIndex, ::nwol::GODS(SBuffer)* out_SplitLeft, ::nwol::GODS(SBuffer)* out_SplitRight) {
	reterr_error_if(0 == in_BufferToSplit, "%s", "Cannot split a null buffer!");
	const ::nwol::SBuffer								* in_instance										= in_BufferToSplit->get();
	uint32_t											leftCount											= nIndex
		,												rightCount											= in_instance->nElementCount - nIndex
		,												typeSize											= GTYPEID_TOTALBYTES(in_instance->DataFormat)
		;
	GPNCO(::nwol, SBuffer)								LeftBuffer
		,												RightBuffer
		;
	bool												isText												= (LeftBuffer->Usage == GUSAGE_TEXT);

	nwol_necall(::nwol::createBuffer(in_instance->DataFormat, in_instance->Usage, leftCount , &LeftBuffer ), "createBuffer() failed for %u elements. Out of memory?", leftCount );
	nwol_necall(::nwol::createBuffer(in_instance->DataFormat, in_instance->Usage, rightCount, &RightBuffer), "createBuffer() failed for %u elements. Out of memory?", rightCount);
	memcpy(LeftBuffer ->pByteArray, in_instance->pByteArray, typeSize * leftCount );
	memcpy(RightBuffer->pByteArray, in_instance->pByteArray, typeSize * rightCount);
	if (isText) {
		uint32_t											elemEnd												= typeSize*leftCount;
		for (uint32_t i = 0; i< typeSize; i++)
			((char*)LeftBuffer->pByteArray)[elemEnd + i]	= 0;
		elemEnd											= typeSize*rightCount;
		for (uint32_t i = 0; i< typeSize; i++)
			((char*)RightBuffer->pByteArray)[elemEnd + i]	= 0;
	}

	::nwol::set(out_SplitLeft	, LeftBuffer	.get_pointer());
	::nwol::set(out_SplitRight	, RightBuffer	.get_pointer());
	return 0;
}

uint32_t									nwol::fileDeserializeData								(GODS(SBuffer)* out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp) {
	retnul_error_if(0 == in_fp || 0 == out_DefinitionList, 
			"Invalid parameters calling fileLoadBufferData():\n"
			"GODS(SBuffer)*	: 0x%p\n"
			"uint32_t		: %i\n"
			"FILE*			: 0x%p"
			, out_DefinitionList
			, (int)nDefinitionCount
			, in_fp
			);

	uint32_t											size1, count1, i
		,												nSkipped											= 0
		;
	uint8_t												bSaved;

	GPNCO(::nwol, SBuffer)								pNewData, pOldData;
	::nwol::galloc(&pNewData);
	for (i = 0; i< nDefinitionCount; i++) {
		retval_error_if(i, fread(&bSaved, sizeof(bSaved), 1, in_fp) != 1, "Failed to read file trying to load definition #%u.", i);
		if (0 == bSaved) {// null buffer, continue!
			++nSkipped;
			::nwol::release(&out_DefinitionList[i]); // clear output
			continue;
		}

		size1											= sizeof(SBuffer) - sizeof(void*)*4;
		retval_error_if(i, fread(pNewData.get_address(), size1, 1, in_fp) != (size_t)1, "Error loading buffer data at index #%u:\n", i);

		GODS(SBuffer)										actualNewBuffer										= 0;
		if (0 != (count1 = pNewData->nElementCount)) {
			::nwol::error_t										errMy												= ::nwol::createBuffer(pNewData->DataFormat, pNewData->Usage, pNewData->nElementCount, &actualNewBuffer);
			retval_error_if(i, errored(errMy), "%s", "createBuffer() FAILED!! Out of memory?");
			SBuffer												* actualInstance									= actualNewBuffer->get();
			actualInstance->nColumnCount					= pNewData->nColumnCount;
			size1											= GTYPEID_TOTALBYTES(actualInstance->DataFormat);
			count1											= actualInstance->nElementCount;
			if(fread(actualInstance->pByteArray, size1, count1, in_fp) != (size_t)count1) {
				error_printf("Error (2) loading byte array for buffer data at index #%u", i);
				::nwol::release(&actualNewBuffer);
				return i;
			}
		}
		pOldData										= out_DefinitionList[i];
		out_DefinitionList[i]							= actualNewBuffer;
	}
	if (pNewData)
		memset(pNewData.get_address(), 0, ::nwol::get_type_size<SBuffer>());

	info_printf("%i GODS(SBuffer) instances read from file, %i skipped.", i - nSkipped, nSkipped);
	return i;
}

uint32_t									nwol::memDeserializeData								(GODS(SBuffer)* out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer) {
	retnul_error_if(0 == in_pMemoryBuffer || 0 == out_DefinitionList, 
			"Invalid parameters calling memLoadBufferData():\n"
			"GODS(SBuffer)*	: 0x%p\n"
			"uint32_t			: %i\n"
			"void*				: 0x%p"
			, out_DefinitionList
			, (int)nDefinitionCount
			, in_pMemoryBuffer
			);

	uint32_t											i, byteIndex										= 0
		,												nSkipped											= 0
		;
	GPNCO(::nwol, SBuffer)								pNewData, pOldData;
	::nwol::galloc(&pNewData);
	for (i = 0; i< nDefinitionCount; i++) {
		uint8_t												bSaved												= ((char*)in_pMemoryBuffer)[byteIndex++];
		if (0 == bSaved) {	// null buffer, continue!
			++nSkipped;
			::nwol::release(&out_DefinitionList[i]); // clear output
			continue;
		}

		uint32_t											size1												= sizeof(SBuffer) - sizeof(void*)*4;

		memcpy(pNewData.get_address(), &((char*)in_pMemoryBuffer)[byteIndex], size1);
		byteIndex										+= size1;

		GODS(SBuffer)										actualNewBuffer										= 0;
		if (0 != pNewData->nElementCount) {
			::nwol::error_t										errMy												= ::nwol::createBuffer(pNewData->DataFormat, pNewData->Usage, pNewData->nElementCount, pNewData->nColumnCount, pNewData->nSliceCount, &actualNewBuffer);
			retval_error_if(byteIndex, errored(errMy), "%s", "createBuffer() FAILED!! Out of memory?");
			SBuffer												* actualInstance									= actualNewBuffer->get();
			size1											= actualInstance->nElementCount*GTYPEID_TOTALBYTES(actualInstance->DataFormat);
			memcpy( actualInstance->pByteArray, &((char*)in_pMemoryBuffer)[byteIndex], size1);
			byteIndex										+= size1;
		}
		pOldData										= out_DefinitionList[i];
		out_DefinitionList[i]							= actualNewBuffer;
	}
	if (pNewData)
		memset(pNewData.get_address(), 0, ::nwol::get_type_size<SBuffer>());

	info_printf("%i _GBUFFER_DATA instances read from memory stream, %i skipped.", i - nSkipped, nSkipped);
	return byteIndex;
}

uint32_t										nwol::fileSerializeData									(GREF(SBuffer)* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp) {
	retnul_error_if(0 == out_fp || 0 == in_DefinitionList, 
			"Invalid parameters calling fileSaveBufferData():\n"
			"GREF(SBuffer)**	: 0x%p\n"
			"uint32_t			: %i\n"
			"FILE*				: 0x%p"
			, in_DefinitionList
			, (int)nDefinitionCount
			, out_fp
			);

	uint32_t											i, nSkipped												= 0
		;
	for (i = 0; i< nDefinitionCount; i++) {
		GODS(SBuffer)										pDef													= in_DefinitionList[i];
		// save a 32 bit boolean telling if the region is gonna be saved to the file (0 for a null region)
		uint8_t												bSavedBuffer											= one_if(pDef);
		uint32_t											size1													= sizeof(bSavedBuffer);
		rve_if(i, fwrite(&bSavedBuffer, size1, 1, out_fp) != 1, "Failed to write metadata to file. %i GODS(SBuffer) instances read from memory stream, %i skipped.", i - nSkipped, nSkipped);
		if (0 == bSavedBuffer) {
			++nSkipped;
			continue;
		}
		size1											= sizeof(SBuffer) - sizeof(void*)*4;
		rve_if(i, fwrite(pDef->get(), size1, 1, out_fp) != 1, "Error saving buffer data at index %u. %i GODS(SBuffer) instances saved to memory stream, %i skipped.", i, i - nSkipped, nSkipped);
		const ::nwol::SBuffer								* instanceToWrite										= pDef->get();
		uint32_t											count1													= instanceToWrite->nElementCount;
		if (0 == count1) 
			continue;

		size1											= GTYPEID_TOTALBYTES(instanceToWrite->DataFormat);
		rve_if(i, fwrite(instanceToWrite->pByteArray, size1, count1, out_fp) != (size_t)count1, "Error (2) saving buffer data at index %i. %i GODS(SBuffer) instances saved to memory stream, %i skipped.", i, i - nSkipped, nSkipped);
	}

	info_printf("%i GODS(SBuffer) instances serialized to file, %i skipped.", i - nSkipped, nSkipped);
	return i;
}

uint32_t										nwol::memSerializeData									(GODS(SBuffer) const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer) {
	retnul_error_if(0 == nDefinitionCount || 0 == in_DefinitionList,	// 
			"Invalid parameters calling memSaveBufferData():\n"
			"GODS(SBuffer)*	: 0x%p\n"
			"uint32_t		: %i\n"
			"void*			: 0x%p"
			, in_DefinitionList
			, (int)nDefinitionCount
			, out_pMemoryBuffer
			);

	uint32_t											i, nSkipped												= 0;
	uint32_t											byteIndex												= 0;
	for (i = 0; i< nDefinitionCount; i++) {
		GODS(SBuffer)										pDef													= in_DefinitionList[i];
		// save a 32 bit boolean telling if the region is gonna be saved to the file (0 for a null region)
		uint8_t												bSaved													= one_if(pDef);
		if (out_pMemoryBuffer)
			((char*)out_pMemoryBuffer)[byteIndex++]			= bSaved;

		if (0 == bSaved) {
			++nSkipped;
			continue;
		}
		uint32_t											size1													= sizeof(SBuffer) - sizeof(void*)*4;
		if (out_pMemoryBuffer) // only write the memory if there is any 
			memcpy(&((char*)out_pMemoryBuffer)[byteIndex], pDef->get(), size1);
		byteIndex										+= size1;

		const ::nwol::SBuffer								* instanceToWrite										= pDef->get();
		if (0 != (size1 = (instanceToWrite->nElementCount*GTYPEID_TOTALBYTES(instanceToWrite->DataFormat)))) {
			if (out_pMemoryBuffer)
				memcpy(&((char*)out_pMemoryBuffer)[byteIndex], instanceToWrite->pByteArray, size1);
			byteIndex										+= size1;
		}
	}
	info_printf("%i GODS(SBuffer) instances serialized to memory, %i skipped.", i - nSkipped, nSkipped);
	return byteIndex;
}