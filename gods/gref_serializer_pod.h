#include "nwol_debug.h"

#ifndef NWOL_GREF_SERIALIZER_POD_H_886465456489
#define NWOL_GREF_SERIALIZER_POD_H_886465456489

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

#if defined(NWOL_DEBUG_ENABLED)
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
	template <typename _tRef> 
	class gref_serializer_pod {
		typedef		typename _tRef::TBase				_tBase;

#if defined(NWOL_DEBUG_ENABLED)
		struct SPODSerializerCounters {
			uint32_t MemSerialize	= 0
				, MemWrite			= 0
				, MemDeserialize	= 0
				, MemRead			= 0
				, FileSerialize		= 0
				, FileWrite			= 0
				, FileDeserialize	= 0
				, FileRead			= 0
				;
		}												Counters;
	public:
														~gref_serializer_pod		()																														{
			if	(	Counters.MemSerialize
				||	Counters.MemWrite
				||	Counters.MemDeserialize
				||	Counters.MemRead
				||	Counters.FileSerialize
				||	Counters.FileWrite
				||	Counters.FileDeserialize
				||	Counters.FileRead
				)
			{
				info_printf("Shutting down serializer for type: %s", _tRef::get_type_name().begin());
			}

			info_if(Counters.MemWrite			, "Instances mem-streamed successfully by this serializer: %u"		,	Counters.MemWrite			);
			info_if(Counters.MemRead			, "Instances mem-streamed successfully by this serializer: %u"		,	Counters.MemRead			);
			info_if(Counters.FileWrite			, "Instances file-streamed successfully by this serializer: %u"		,	Counters.FileWrite			);
			info_if(Counters.FileRead			, "Instances file-streamed successfully by this serializer: %u"		,	Counters.FileRead			);
			info_if(Counters.MemSerialize		, "Instances mem-serialized successfully by this serializer: %u"	,	Counters.MemSerialize		);
			info_if(Counters.MemDeserialize		, "Instances mem-serialized successfully by this serializer: %u"	,	Counters.MemDeserialize		);
			info_if(Counters.FileSerialize		, "Instances file-serialized successfully by this serializer: %u"	,	Counters.FileSerialize		);
			info_if(Counters.FileDeserialize	, "Instances file-serialized successfully by this serializer: %u"	,	Counters.FileDeserialize	);
		}
	private:
#endif
	public:
		static		gref_serializer_pod<_tRef>&			get							()																														{
			static	gref_serializer_pod<_tRef>					instance;
			return instance;
		}
		//--------------------------------------- NEW REFERENCE SYSTEM
					uint32_t							memWriteData				(_tRef* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer, const _tBase* in_DefaultData)		{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == nDefinitionCount || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"void*				: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_pMemoryBuffer
					);
			::nwol::gptr_pod<_tRef>								defaultData					= in_DefaultData;
			if (0 == in_DefaultData) {
				defaultData.create();
				if (!defaultData) {
					error_printf("gcreate() returned NULL! Out of memory? Cannot write data to file.");
					return 0;
				}
				memset(defaultData.get_address(), 0, sizeof(_tBase));
			}
			uint32_t											i, nSkipped = 0, byteIndex	= 0;
			_tRef												* pInstanceToSave			= 0;
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
			info_printf("%i %s instances written to memory stream, %i written with default values.", i, typeName.begin(), nSkipped);
			//i_ = 0;//??
			PLATFORM_CRT_CHECK_MEMORY();
			return byteIndex;
		}	// method

					uint32_t							memReadData					(_tRef** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == in_pMemoryBuffer
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
			::nwol::gptr_pod<_tRef>								newData, oldData;
			::nwol::glist_pod<_tRef>							lstLoadedData(nDefinitionCount);
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
			info_printf("%i %s instances read from memory stream, %i referenced.", i, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							memSerializeData			(_tRef* const* in_DefinitionList, uint32_t nDefinitionCount, void* out_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == nDefinitionCount || 0 == in_DefinitionList
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
			info_printf("%i %s instances written to memory stream, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							memDeserializeData			(_tRef** out_DefinitionList, uint32_t nDefinitionCount, const void* in_pMemoryBuffer)									{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == in_pMemoryBuffer
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
			::nwol::gptr_pod<_tRef>								newData, oldData;
			for (i = 0; i< nDefinitionCount; i++) {
				PLATFORM_CRT_CHECK_MEMORY();
				bSaved											= ((char*)in_pMemoryBuffer)[byteIndex]; // read boolean telling if there's data or not
				byteIndex++;
				if (0 == bSaved) { // null definition, continue! 
					++nSkipped;
					typedef void(*funcType)(_tRef**);			
					if (out_DefinitionList)
						::nwol::release(&out_DefinitionList[i]); // clear output
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
			info_printf("%i %s instances read from memory stream, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return byteIndex;
		} // method

					uint32_t							fileWriteData				(_tRef* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp, const _tBase* in_DefaultData)				{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == out_fp || 0 == in_DefinitionList
					,	"Invalid parameters!\n"
						"%s**				: 0x%p\n"
						"uint32_t			: %i\n"
						"FILE*				: 0x%p"
					, typeName.begin()
					, in_DefinitionList
					, (int)nDefinitionCount
					, out_fp
					);

			::nwol::gptr_pod<_tRef>								instanceToSave
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
			info_printf("%i %s instances written to file, %i written with default values.", i, typeName.begin(), nSkipped);
			PLATFORM_CRT_CHECK_MEMORY();
			return i;
		} // method

					uint32_t							fileReadData				(_tRef** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp)													{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == in_fp
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
			::nwol::gptr_pod<_tRef>								newData, oldData;
			::nwol::glist_pod<_tRef>							lstLoadedData				(nDefinitionCount);
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
			info_printf("%i %s instances read from file, %i referenced.", i, typeName.begin(), nSkipped);
			return i;
		} // method

					uint32_t							fileDeserializeData			(_tRef** out_DefinitionList, uint32_t nDefinitionCount, FILE* in_fp)													{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == in_fp
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

			::nwol::gptr_pod<_tRef>								newData, oldData;
			for (i = 0; i< nDefinitionCount; i++){
				if (fread(&bSaved, sizeof(bSaved), 1, in_fp) < 1) {
					error_printf("Failed to read metadata from file.");
					return i;
				}
				if (0 == bSaved) {// null buffer, continue!
					++nSkipped;
					typedef void(*funcType)(_tRef**);			
					if(out_DefinitionList[i])
						::nwol::release(&out_DefinitionList[i]); /*clear output*/
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
			info_printf("%i %s instances read from file, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return i;
		} // method

					uint32_t							fileSerializeData			(_tRef* const* in_DefinitionList, uint32_t nDefinitionCount, FILE* out_fp)												{
			PLATFORM_CRT_CHECK_MEMORY();
			static const ::nwol::glabel							& typeName					= _tRef::get_type_name();
			retnul_error_if(0 == out_fp || 0 == in_DefinitionList
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
			info_printf("%i %s instances written to file, %i skipped.", i - nSkipped, typeName.begin(), nSkipped);
			return i;
		} // method

	}; // struct

} // namespace

#endif // NWOL_GREF_SERIALIZER_POD_H_886465456489

