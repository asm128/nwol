#include "nwol_debug.h"

#ifndef __GREF_SERIALIZER_POD_H_886465456489__
#define __GREF_SERIALIZER_POD_H_886465456489__

#define __GDEFINE_PRINTGENERIC_FUNCTION( baseType, refType, format, ... )																																	\
void printInfoString	( const refType* in_CoreInstance )													{	printInfoString( in_CoreInstance ? in_CoreInstance->get() : 0 );	}						\
void getInfoString		( char* pOutputBuffer, uint32_t nBufferSize, const refType* in_CoreInstance )		{	getInfoString(pOutputBuffer, nBufferSize, in_CoreInstance ? in_CoreInstance->get() : 0); }	\
void getInfoString		( wchar_t* pOutputBuffer, uint32_t nBufferSize, const refType* in_CoreInstance )	{	getInfoString(pOutputBuffer, nBufferSize, in_CoreInstance ? in_CoreInstance->get() : 0); }	\
void printInfoString	( const baseType* Data )															{																								\
	if( 0 == Data )	{																																														\
		warning_printf("[<"#baseType"*>=NULL]");																																							\
		return;																																																\
	}																																																		\
	data_printf("[<"#baseType"*>=0x%p]:\n"																																									\
		format																																																\
		, Data																																																\
		, __VA_ARGS__																																														\
	);																																																		\
}

#if defined(ANDROID) || defined(__linux__)
//---------------------
#define GDEFINE_PRINTGENERIC( baseType, refType, format, ... )								\
__GDEFINE_PRINTGENERIC_FUNCTION( baseType, refType, format, __VA_ARGS__ )					\
void getInfoString( char* pOutputBuffer, uint32_t nBufferSize, const baseType* Data )		\
{																							\
	if( 0 == Data )																			\
	{																						\
		sprintf( pOutputBuffer, "%s\n", "[<" #baseType "*>=NULL]" );						\
		return;																				\
	}																						\
	sprintf( pOutputBuffer, format "\n", __VA_ARGS__	);									\
}																							\
void getInfoString( wchar_t* pOutputBuffer, uint32_t nBufferSize, const baseType* Data )	\
{																							\
	if( 0 == Data )																			\
	{																						\
		/*swprintf( pOutputBuffer, L"%s\n", L"(null)" );*/									\
		return;																				\
	}																						\
	char buffer[2048];																		\
	sprintf( buffer, format "\n", __VA_ARGS__	);											\
	mbstowcs( pOutputBuffer, buffer, 2048 );												\
}
#else
//---------------------
#define GDEFINE_PRINTGENERIC( baseType, refType, format, ... )								\
__GDEFINE_PRINTGENERIC_FUNCTION( baseType, refType, format, __VA_ARGS__ )					\
void getInfoString( char* pOutputBuffer, uint32_t nBufferSize, const baseType* Data ) {		\
	if( 0 == Data )	{																		\
		sprintf_s( pOutputBuffer, nBufferSize, "%s\n", "[<"#baseType"*>=NULL]" );			\
		return;																				\
	}																						\
	sprintf_s( pOutputBuffer, nBufferSize, format "\n", __VA_ARGS__	);						\
}																							\
void getInfoString( wchar_t* pOutputBuffer, uint32_t nBufferSize, const baseType* Data ) {	\
	if( 0 == Data )	{																		\
		swprintf_s( pOutputBuffer, nBufferSize, L"%s\n", L"[<"#baseType"*>=NULL]" );		\
		return;																				\
	}																						\
	char buffer[2048];																		\
	sprintf_s( buffer, nBufferSize, format "\n", __VA_ARGS__ );								\
	size_t n;																				\
	mbstowcs_s( &n, pOutputBuffer, nBufferSize, buffer, 2048 );								\
}
#endif // ANDROID

#define GDEFINE_PRINTPOD( NameSpace, baseType, format, ... )	namespace NameSpace{ GDEFINE_PRINTGENERIC( baseType, GREF(baseType), format,   __VA_ARGS__ )	}
#define GDEFINE_PRINTOBJ( NameSpace, baseType, format, ... )	namespace NameSpace{ GDEFINE_PRINTGENERIC( baseType, GREF(baseType), format, __VA_ARGS__ )		}
#define GDEFINE_PRINTPOD_NONS( baseType, format, ... )			GDEFINE_PRINTGENERIC( baseType, GREF(baseType), format,   __VA_ARGS__ )
#define GDEFINE_PRINTOBJ_NONS( baseType, format, ... )			GDEFINE_PRINTGENERIC( baseType, GREF(baseType), format, __VA_ARGS__ )

#define __GDEFINE_POD_MEMWRITE( baseType )			uint32_t memWriteData			( GREF(baseType)* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer, const baseType* in_DefaultData )	{ return __g_##baseType##Serializer.memWriteData		( in_DefinitionList, nDefinitionCount, out_pMemoryBuffer, in_DefaultData );	}
#define __GDEFINE_POD_MEMREAD( baseType )			uint32_t memReadData			( GREF(baseType)** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer )									{ return __g_##baseType##Serializer.memReadData			( out_DefinitionList, nDefinitionCount, in_pMemoryBuffer );					}
#define __GDEFINE_POD_MEMSERIALIZE( baseType )		uint32_t memSerializeData		( GREF(baseType)* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer )									{ return __g_##baseType##Serializer.memSerializeData	( in_DefinitionList, nDefinitionCount, out_pMemoryBuffer );					}
#define __GDEFINE_POD_MEMDESERIALIZE( baseType )	uint32_t memDeserializeData		( GREF(baseType)** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer )									{ return __g_##baseType##Serializer.memDeserializeData	( out_DefinitionList, nDefinitionCount, in_pMemoryBuffer );					}
#define __GDEFINE_POD_FILEWRITE( baseType )			uint32_t fileWriteData			( GREF(baseType)* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp, const baseType* in_DefaultData )				{ return __g_##baseType##Serializer.fileWriteData		( in_DefinitionList, nDefinitionCount, out_fp, in_DefaultData );			}
#define __GDEFINE_POD_FILEREAD( baseType )			uint32_t fileReadData			( GREF(baseType)** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp )														{ return __g_##baseType##Serializer.fileReadData		( out_DefinitionList, nDefinitionCount, in_fp );							}
#define __GDEFINE_POD_FILEDESERIALIZE( baseType )	uint32_t fileDeserializeData	( GREF(baseType)** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp )														{ return __g_##baseType##Serializer.fileDeserializeData	( out_DefinitionList, nDefinitionCount, in_fp );							}
#define __GDEFINE_POD_FILESERIALIZE( baseType )		uint32_t fileSerializeData		( GREF(baseType)* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp )												{ return __g_##baseType##Serializer.fileSerializeData	( in_DefinitionList, nDefinitionCount, out_fp );							}
//----------------------------------------------------------------------

#define __GDEFINE_FILEMEM_SERIALIZATION_FUNCTIONS( baseType)	\
	__GDEFINE_POD_FILESERIALIZE		(baseType);					\
	__GDEFINE_POD_FILEDESERIALIZE	(baseType);					\
	__GDEFINE_POD_MEMSERIALIZE		(baseType);					\
	__GDEFINE_POD_MEMDESERIALIZE	(baseType);

#define __GDEFINE_FILEMEM_STREAMING_FUNCTIONS(baseType)	\
	__GDEFINE_POD_MEMWRITE	(baseType);					\
	__GDEFINE_POD_MEMREAD	(baseType);					\
	__GDEFINE_POD_FILEWRITE	(baseType);					\
	__GDEFINE_POD_FILEREAD	(baseType);

//--------------------------------------------- Streaming function macros
#if defined(ANDROID) || defined(__linux__)
#include "glist.h"
#endif

#if defined(_DEBUG) || defined(DEBUG)
#	define	INCREASECOUNTERMEMSERIALIZE			Counters.MemSerialize		++
#	define	INCREASECOUNTERMEMWRITE				Counters.MemWrite			++
#	define	INCREASECOUNTERMEMDESERIALIZE		Counters.MemDeserialize		++
#	define	INCREASECOUNTERMEMREAD				Counters.MemRead			++
#	define	INCREASECOUNTERFILESERIALIZE		Counters.FileSerialize		++
#	define	INCREASECOUNTERFILEWRITE			Counters.FileWrite			++
#	define	INCREASECOUNTERFILEDESERIALIZE		Counters.FileDeserialize	++
#	define	INCREASECOUNTERFILEREAD				Counters.FileRead			++
#else
#	define	INCREASECOUNTERMEMSERIALIZE		
#	define	INCREASECOUNTERMEMWRITE
#	define	INCREASECOUNTERMEMDESERIALIZE
#	define	INCREASECOUNTERMEMREAD
#	define	INCREASECOUNTERFILESERIALIZE
#	define	INCREASECOUNTERFILEWRITE
#	define	INCREASECOUNTERFILEDESERIALIZE
#	define	INCREASECOUNTERFILEREAD
#endif
namespace nwol
{
	template <typename _TRef> class gref_serializer_pod
	{
		typedef typename _TRef::TBase _tBase;

#if defined(_DEBUG) || defined(DEBUG)
		struct SPODSerializerCounters
		{
		public:
			uint32_t MemSerialize	= 0
				, MemWrite			= 0
				, MemDeserialize	= 0
				, MemRead			= 0
				, FileSerialize		= 0
				, FileWrite			= 0
				, FileDeserialize	= 0
				, FileRead			= 0
				;
		} Counters;

	public:
														~gref_serializer_pod		()																														{
			if(	Counters.MemSerialize
				||	Counters.MemWrite
				||	Counters.MemDeserialize
				||	Counters.MemRead
				||	Counters.FileSerialize
				||	Counters.FileWrite
				||	Counters.FileDeserialize
				||	Counters.FileRead
				)
			{
				debug_printf("Shutting down serializer for type: %s", _TRef::get_type_name().begin());
			}

			if (Counters.MemSerialize)		{ debug_printf("Instances mem serialized successfully by this serializer: %u"	,	Counters.MemSerialize		);	}
			if (Counters.MemWrite)			{ debug_printf("Instances mem streamed successfully by this serializer: %u"		,	Counters.MemWrite			);	}
			if (Counters.MemDeserialize)	{ debug_printf("Instances mem serialized successfully by this serializer: %u"	,	Counters.MemDeserialize		);	}
			if (Counters.MemRead)			{ debug_printf("Instances mem streamed successfully by this serializer: %u"		,	Counters.MemRead			);	}
			if (Counters.FileSerialize)		{ debug_printf("Instances file serialized successfully by this serializer: %u"	,	Counters.FileSerialize		);	}
			if (Counters.FileWrite)			{ debug_printf("Instances file streamed successfully by this serializer: %u"	,	Counters.FileWrite			);	}
			if (Counters.FileDeserialize)	{ debug_printf("Instances file serialized successfully by this serializer: %u"	,	Counters.FileDeserialize	);	}
			if (Counters.FileRead)			{ debug_printf("Instances file streamed successfully by this serializer: %u"	,	Counters.FileRead			);	}
		}
	private:
#endif
	public:
		static		gref_serializer_pod<_TRef>&			get							()																														{
			static gref_serializer_pod<_TRef>					instance;
			return instance;
		}
		//--------------------------------------- NEW REFERENCE SYSTEM
					uint32_t							memWriteData				(_TRef* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer, const _tBase* in_DefaultData)		{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == nDefinitionCount || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"void*				: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_pMemoryBuffer
					);
			::nwol::gptr_pod<_TRef>								defaultData					= in_DefaultData;
			if (0 == in_DefaultData) {
				defaultData.create();
				if (!defaultData) {
					error_printf("gcreate() returned NULL! Out of memory? Cannot write data to file.");
					return 0;
				}
				memset(defaultData.get_address(), 0, sizeof(_tBase));
			}
			uint32_t											i, nSkipped = 0, byteIndex	= 0;
			_TRef												* pInstanceToSave			= 0;
			for (i = 0; i< nDefinitionCount; i++) {
				PLATFORM_CRT_CHECK_MEMORY();
				// Select instance values or default values if instance is NULL
				if (0 == in_DefinitionList[i]) {
					pInstanceToSave									= defaultData;
					nSkipped++;
				}
				else
					pInstanceToSave									= in_DefinitionList[i];
				if (out_pMemoryBuffer) {
					memcpy(&((char*)out_pMemoryBuffer)[byteIndex], pInstanceToSave->get(), sizeof(_tBase));
					INCREASECOUNTERMEMWRITE;
				}
				byteIndex									+= sizeof(_tBase);
			}
			debug_printf("%i %s instances written to memory stream, %i written with default values.", i, typeName.begin(), nSkipped);
			//i_ = 0;//??
			PLATFORM_CRT_CHECK_MEMORY();
			return byteIndex;
		}	// method

					uint32_t							memReadData					(_TRef** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == in_pMemoryBuffer
					,	"Invalid parameters!"
						"%s**			: 0x%p\n"
						"uint32_t		: %i\n"
						"void*			: 0x%p"
					, typeName.begin()
					, out_DefinitionList
					, (int)nDefinitionCount
					, in_pMemoryBuffer
					);
			uint32_t											i, j
				,												nLoaded		= 0
				,												byteIndex	= 0
				,												nSkipped	= 0
				;
			::nwol::gptr_pod<_TRef>								newData, oldData;
			::nwol::glist_pod<_TRef>							lstLoadedData(nDefinitionCount);
			for (i = 0; i< nDefinitionCount; i++) {
				PLATFORM_CRT_CHECK_MEMORY();
				newData.create();
				if (!newData) {
					error_printf("Cannot create new %s instance! Terminating function...", typeName.begin());
					return byteIndex;
				}
				memcpy(newData.get_address(), &((char*)in_pMemoryBuffer)[byteIndex], sizeof(_tBase));
				PLATFORM_CRT_CHECK_MEMORY();
				byteIndex										+= sizeof(_tBase);
				for (j = 0; j<nLoaded; j++)
					if (0 == ::nwol::podcmp(lstLoadedData[j], newData.get_address())) {
						newData											= lstLoadedData.acquire(j);
						nSkipped++;
						break;
					}
				if (j == nLoaded) {
					lstLoadedData.begin()[nLoaded] = newData.acquire();
					nLoaded++;
				}
				if (out_DefinitionList) {
					oldData											= out_DefinitionList[i];
					out_DefinitionList[i]							= newData.acquire();
					INCREASECOUNTERMEMREAD;
				}
			}
			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances read from memory stream, %i referenced.", i, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							memSerializeData			(_TRef* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == nDefinitionCount || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**					: 0x%p\n"
						"uint32_t				: %i\n"
						"void*					: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_pMemoryBuffer
					);
			uint32_t											i
				,												nSkipped					= 0
				,												byteIndex					= 0;
			uint8_t bSaved;
			for (i = 0; i< nDefinitionCount; i++) {
				// save a byte boolean telling if the data is gonna be saved to the file (0 for a null data) 
				PLATFORM_CRT_CHECK_MEMORY();
				bSaved											= one_if(in_DefinitionList[i]);
				if (out_pMemoryBuffer)
					((char*)out_pMemoryBuffer)[byteIndex]			= bSaved;
				byteIndex++;
				if (0 == bSaved) {
					nSkipped++;
					continue;
				}
				if (out_pMemoryBuffer) {
					memcpy(&((char*)out_pMemoryBuffer)[byteIndex], in_DefinitionList[i]->get(), sizeof(_tBase));
					INCREASECOUNTERMEMSERIALIZE;
				}
				byteIndex										+= sizeof(_tBase);
			}
			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances written to memory stream, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							memDeserializeData			(_TRef** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == in_pMemoryBuffer
					,	"Invalid parameters!"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"void*				: 0x%p"
					, typeName.begin()
					, out_DefinitionList
					, (int)nDefinitionCount
					, in_pMemoryBuffer
					);

			uint32_t											i, nSkipped = 0, byteIndex	= 0;
			uint8_t bSaved;
			::nwol::gptr_pod<_TRef>								newData, oldData;
			for (i = 0; i< nDefinitionCount; i++) {
				PLATFORM_CRT_CHECK_MEMORY();
				bSaved											= ((char*)in_pMemoryBuffer)[byteIndex]; // read boolean telling if there's data or not
				byteIndex++;
				if (0 == bSaved) { // null definition, continue! 
					++nSkipped;
					typedef void(*funcType)(_TRef**);			
					if (out_DefinitionList)
						((funcType)out_DefinitionList[i]->Globals->__prelease)(&out_DefinitionList[i]); // clear output
					continue;
				}
				// Create new instance to store data
				newData.create();
				if (!newData) {
					error_printf("Cannot create new %s instance! Terminating function...", typeName.begin());
					return byteIndex;
				}
				memcpy(newData.get_address(), &((char*)in_pMemoryBuffer)[byteIndex], sizeof(_tBase));
				byteIndex										+= sizeof(_tBase);
				if (out_DefinitionList) {
					oldData											= out_DefinitionList[i];
					out_DefinitionList[i]							= newData.acquire();
					INCREASECOUNTERMEMDESERIALIZE;
				}
			}
			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances read from memory stream, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							fileWriteData				(_TRef* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp, const _tBase* in_DefaultData)				{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == out_fp || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"FILE*				: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_fp
					);

			::nwol::gptr_pod<_TRef>								instanceToSave
				,												defaultData					= in_DefaultData
				;
			if (0 == in_DefaultData) {
				defaultData.create();
				if (!defaultData) {
					error_printf("Out of memory? Cannot write data to file.");
					return 0;
				}
				memset(defaultData.get_address(), 0, sizeof(_tBase));
			}
			uint32_t											i, nSkipped					= 0;
			for (i = 0; i< nDefinitionCount; i++) {
			PLATFORM_CRT_CHECK_MEMORY();
				// Select instance values or default values if instance is NULL 
				if (0 == in_DefinitionList[i]) {
					instanceToSave									= defaultData;
					nSkipped++;
				}
				else
					instanceToSave.acquire(in_DefinitionList[i]);

				if (fwrite(instanceToSave.get_address(), sizeof(_tBase), 1, out_fp) < 1) {
					error_printf("Error saving %s instance at location %p:\n"
						, typeName.begin()
						, in_DefinitionList[i]
						);
					error_printf("%i %s instances written to file.", i, typeName.begin());
					return i;
				}
				INCREASECOUNTERFILEWRITE;
			}
			debug_printf("%i %s instances written to file, %i written with default values.", i, typeName.begin(), nSkipped);
			PLATFORM_CRT_CHECK_MEMORY();
			return i;
		} // method

					uint32_t							fileReadData				(_TRef** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp)													{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == in_fp
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"FILE*				: 0x%p"
					, typeName.begin()
					, out_DefinitionList
					, (int)nDefinitionCount
					, in_fp
					);

			uint32_t											i, j, nLoaded = 0, nSkipped	= 0;
			::nwol::gptr_pod<_TRef>								newData, oldData;
			::nwol::glist_pod<_TRef>							lstLoadedData				(nDefinitionCount);
			for (i = 0; i< nDefinitionCount; i++) {
				PLATFORM_CRT_CHECK_MEMORY();
				newData.create();
				if (!newData) {
					error_printf("Cannot create new %s instance! Terminating function...", typeName.begin());
					return i;
				}
				if (fread(newData.get_address(), sizeof(_tBase), 1, in_fp) < 1) {
					error_printf("Cannot read %s at position %i:\n"
						, typeName.begin()
						, (int)i
						);
					error_printf("%i %s instances read from file.", i, typeName.begin());
					return i;
				}
				for (j = 0; j<nLoaded; j++) {
					if (0 == newData.compare_data(lstLoadedData.begin()[j])) {
						newData											= lstLoadedData.acquire(j);
						nSkipped++;
						break;
					}
				}
				if (j == nLoaded){
					lstLoadedData.begin()[nLoaded]					= newData.acquire();
					nLoaded++;
				}
				if (out_DefinitionList) {
					oldData											= out_DefinitionList[i];
					out_DefinitionList[i]							= newData.acquire();
					INCREASECOUNTERFILEREAD;
				}
			}
			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances read from file, %i referenced.", i, typeName.begin(), nSkipped);
			return i;
		} // method

					uint32_t							fileDeserializeData			(_TRef** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp)													{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == in_fp
					,	"Invalid parameters!\n"
						"%s**		: 0x%p\n"
						"uint32_t			: %i\n"
						"FILE*				: 0x%p"
					, typeName.begin()
					, out_DefinitionList
					, (int)nDefinitionCount
					, in_fp
					);

			uint32_t											i, nSkipped					= 0;
			uint8_t												bSaved;

			::nwol::gptr_pod<_TRef>								newData, oldData;
			for (i = 0; i< nDefinitionCount; i++){
				if (fread(&bSaved, sizeof(bSaved), 1, in_fp) < 1) {
					error_printf("Failed to read metadata from file.");
					return i;
				}
				if (0 == bSaved) {// null buffer, continue!
					++nSkipped;
					typedef void(*funcType)(_TRef**);			
					if(out_DefinitionList[i])
						((funcType)out_DefinitionList[i]->Globals->__prelease)(&out_DefinitionList[i]); /*clear output*/
					continue;
				}
				newData.create();
				if (!newData) {
					error_printf("Cannot create new %s instance! Terminating function...", typeName.begin());
					return i;
				}
				if (fread(newData.get_address(), sizeof(_tBase), 1, in_fp) < 1) {
					error_printf("Cannot read %s at position %i:\n"
						, typeName.begin()
						, (int)i
						);
					error_printf("%i %s instances read from file.", i - nSkipped, typeName.begin());
					return i;
				}
				if (out_DefinitionList) {
					oldData											= out_DefinitionList[i];
					out_DefinitionList[i]							= newData.acquire();
					INCREASECOUNTERFILEDESERIALIZE;
				}
			}
			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances read from file, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return i;
		} // method

					uint32_t							fileSerializeData			(_TRef* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp)												{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _TRef::get_type_name();
			retnul_msg_if(0 == out_fp || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"FILE*				: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_fp
					);

			uint32_t											i, nSkipped					= 0;
			uint8_t												bSaved;
			for (i = 0; i< nDefinitionCount; i++) {
				bSaved											= one_if(in_DefinitionList[i]);
				if (fwrite(&bSaved, sizeof(bSaved), 1, out_fp) < 1) {
					error_printf("Failed to write metadata to file.");
					return i;
				}
				
				if (0 == bSaved) {
					nSkipped++;
					continue;
				}

				if (fwrite(in_DefinitionList[i]->Instance, sizeof(_tBase), 1, out_fp) < 1) {
					error_printf("Error saving %s instance at location %p:\n"
						, typeName.begin()
						, in_DefinitionList[i]
						);
					return i;
				}
				INCREASECOUNTERFILESERIALIZE;
			}

			PLATFORM_CRT_CHECK_MEMORY();
			debug_printf("%i %s instances written to file, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return i;
		} // method

	}; // struct

} // namespace

#endif // __GREF_SERIALIZER_POD_H_886465456489__

