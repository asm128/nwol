/// File:GCore_macros.h
///
/// Contains macros for declaring the core functions and structures of the system.
///
/// This file is part of the Game Object Data System.
/// 2012-2013 (c) Pablo Ariel Zorrilla Cepeda
///

#include "gcore_ref.h"
#include "type_registry.h"
#include "nwol_memory.h"

#ifndef __NWOL_MACROS_H__2398429385492835498234__
#define __NWOL_MACROS_H__2398429385492835498234__

//-----------------------------------------------------------------------------// //--------------------------------------------------------------------------//
#define __GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType)																\
	void gcreate				(GREF(baseType)** inout_pCoreInstance);												\
	void gcreate				(GREF(baseType)** inout_pCoreInstance, const baseType& in_InitData);				\
	void gcreateAll				(GREF(baseType)** inout_pCoreInstance, uint32_t nCount);							\
	void gcreateAll				(GREF(baseType)** p2, const baseType* lstInstances, uint32_t nCount);				\
	void gcopy					(GREF(baseType)* dst_Target  , const GREF(baseType)* src_Data);						\
	void gclone					(GREF(baseType)** out_pTarget, const GREF(baseType)* src_Data);	

// This macro declares functions to retrieve formatted informative text about the structures. These must be coded individually for each structure 
#define __GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS(baseType)															\
	void printInfoString		(const baseType* in_Data);															\
	void printInfoString		(const GREF(baseType)* in_Data);													\
	void getInfoString			(wchar_t* dst_pOutputBuffer, uint32_t nBufferSize,	const baseType* in_Data);		\
	void getInfoString			(wchar_t* dst_pOutputBuffer, uint32_t nBufferSize,	const GREF(baseType)* in_Data);	\
	void getInfoString			(char* dst_pOutputBuffer, uint32_t nBufferSize,	const GREF(baseType)* in_Data);		\
	void getInfoString			(char* dst_pOutputBuffer, uint32_t nBufferSize,	const baseType* in_Data);

#define __GDECLARE_POD_MEMORY_FUNCTIONS(baseType)																	\
	__GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType);																	\
	static inline	int32_t		gcompare	(GREF(baseType)* in_A, GREF(baseType)* in_B)	{ 						\
		if (0 == in_A && 0 == in_B)																					\
			return 0;																								\
		else if (0 == in_A || 0 == in_B)																			\
			return 0x7FFFFFFF;																						\
																													\
		return ::nwol::podcmp(in_A->Instance, in_B->Instance);														\
	}

// This macro is used to declare read/write functions of the structure data (serialization)
#define __GDECLARE_POD_SERIALIZATION_FUNCTIONS(baseType)																											\
	uint32_t fileDeserializeData	(GREF(baseType)** out_lstDataInstances, uint32_t in_nInstanceCount, FILE* in_fp);												\
	uint32_t fileSerializeData		(GREF(baseType)* const* in_lstDataInstances, uint32_t in_nInstanceCount, FILE* out_fp);											\
	uint32_t memDeserializeData		(GREF(baseType)** out_lstDataInstances, uint32_t in_nInstanceCount, const void* in_pMemoryBuffer);								\
	uint32_t memSerializeData		(GREF(baseType)* const* in_lstDataInstances, uint32_t in_nInstanceCount, void* out_pMemoryBuffer);

#define __GDECLARE_POD_STREAMING_FUNCTIONS( baseType )																												\
	uint32_t fileReadData			(GREF(baseType)** out_lstDataInstances, uint32_t in_nInstanceCount, FILE* in_fp);												\
	uint32_t fileWriteData			(GREF(baseType)* const* in_lstDataInstances, uint32_t in_nInstanceCount, FILE* out_fp, const baseType* DefaultData);			\
	uint32_t memReadData			(GREF(baseType)** out_lstDataInstances, uint32_t in_nInstanceCount, const void* in_pMemoryBuffer);								\
	uint32_t memWriteData			(GREF(baseType)* const* in_lstDataInstances, uint32_t in_nInstanceCount, void* out_pMemoryBuffer, const baseType* DefaultData);

// This macro is used to declare all core functions at once
#define __GDECLARE_POD_FUNCTIONS(baseType)					\
	__GDECLARE_POD_MEMORY_FUNCTIONS				(baseType);	\
	__GDECLARE_POD_STREAMING_FUNCTIONS			(baseType);	\
	__GDECLARE_POD_SERIALIZATION_FUNCTIONS		(baseType);	\
	__GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS	(baseType);	\
	static inline	void	onCreateDefault##baseType	(baseType* stuff)	{ *stuff = {}; }

#define GDECLARE_POD_CUSTOMPAGE(baseType, ...)				\
	GDECLARE_REF(baseType, __VA_ARGS__);					\
	__GDECLARE_POD_FUNCTIONS(baseType);

#define GDECLARE_POD(baseType, ...)	GDECLARE_POD_CUSTOMPAGE(baseType, __VA_ARGS__)

#define NWOL_POD(nameSpace, baseType, displayName, descriptionText, ...)																													\
	NWOL_REGISTER_POD(nameSpace, baseType, displayName, descriptionText);																													\
	GDECLARE_REF(baseType, __VA_ARGS__);																				\
	__GDECLARE_POD_FUNCTIONS(baseType);


//-----------------------------------------------------------------------------// //--------------------------------------------------------------------------//
#define GDECLARE_PURE_NO_TYPEDEFS(baseType)					\
	__GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS(baseType);	

#define GDECLARE_NCO_NO_TYPEDEFS(baseType)					\
	__GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS(baseType);		\
	void galloc(GREF(baseType)** inout_pCoreInstance);		\
	void gallocAll(GREF(baseType)** inout_pCoreInstance, uint32_t nCount);

#define GDECLARE_PURE(baseType)						\
	GDECLARE_REF(baseType, (::nwol::GDATA_TYPE)0);	\
	GDECLARE_PURE_NO_TYPEDEFS(baseType);

#define GDECLARE_NCO_CUSTOMPAGE(baseType)																		\
	GDECLARE_REF(baseType, ::nwol::GDATA_TYPE_GNCO);															\
	GDECLARE_NCO_NO_TYPEDEFS(baseType);																			\
	template<typename... _Args> void gcreate(GREF(baseType)** pRef, _Args&&... args) {							\
		galloc(pRef);																							\
		if(pRef)																								\
			new ((*pRef)->get()) GREF(baseType)::TBase(args...);												\
		else																									\
			error_printf("Failed to allocate memory for type: %s.", GREF(baseType)::get_type_name().begin());	\
	}

#define GDECLARE_NCO(baseType)	 GDECLARE_NCO_CUSTOMPAGE(baseType)

#define GDECLARE_OBJ_CUSTOMPAGE(baseType)										\
	GDECLARE_REF						(baseType, ::nwol::GDATA_TYPE_GOBJ);	\
	GDECLARE_NCO_NO_TYPEDEFS			(baseType);								\
	__GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType);

#define GDECLARE_OBJ(baseType)	GDECLARE_OBJ_CUSTOMPAGE(baseType)


#endif // __NWOL_MACROS_H__2398429385492835498234__
