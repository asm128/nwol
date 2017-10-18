/// File:gref.h
///
/// Contains macros for declaring the core functions and structures of the system.
///
/// This file is part of the Game Object Data System.
/// 2012-2017 (c) Pablo Ariel Zorrilla Cepeda
///
#include "gcore_ref.h"
#include "gref_manager.h"

#include "nwol_type_registry.h"
#include "nwol_memory.h"

#ifndef GREF_H_2398429385492835498234
#define GREF_H_2398429385492835498234

//-----------------------------------------------------------------------------// //--------------------------------------------------------------------------//
#define __GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType)																	\
	void		gcreate					(GREF(baseType)** inout_pCoreInstance);											\
	void		gcreate					(GREF(baseType)** inout_pCoreInstance, const baseType& in_InitData);			\
	void		gcreateAll				(GREF(baseType)** inout_pCoreInstance, uint32_t nCount);						\
	void		gcreateAll				(GREF(baseType)** p2, const baseType* lstInstances, uint32_t nCount);			\

// This macro declares functions to retrieve formatted informative text about the structures. These must be coded individually for each structure 
#define __GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS(baseType)																\
	void		printInfoString			(const baseType* in_Data);														\
	void		printInfoString			(const GREF(baseType)* in_Data);												\
	void		getInfoString			(char* dst_pOutputBuffer, uint32_t nBufferSize,	const GREF(baseType)* in_Data);	\
	void		getInfoString			(char* dst_pOutputBuffer, uint32_t nBufferSize,	const baseType* in_Data);

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

#define GDECLARE_NCO(baseType)																					\
	GDECLARE_REF(baseType, ::nwol::GDATA_TYPE_GNCO);															\
	GDECLARE_NCO_NO_TYPEDEFS(baseType);																			\
	template<typename... _Args> void gcreate(GREF(baseType)** pRef, _Args&&... args) {							\
		galloc(pRef);																							\
		if(pRef)																								\
			new ((*pRef)->get()) GREF(baseType)::TBase(args...);												\
		else																									\
			error_printf("Failed to allocate memory for type: %s.", GREF(baseType)::get_type_name().begin());	\
	}

#define GDECLARE_OBJ(baseType)													\
	GDECLARE_REF						(baseType, ::nwol::GDATA_TYPE_GOBJ);	\
	GDECLARE_NCO_NO_TYPEDEFS			(baseType);								\
	__GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType);

#define __GDECLARE_POD_MEMORY_FUNCTIONS(baseType)		__GDECLARE_COMMON_COPIABLE_FUNCTIONS(baseType);

// This macro is used to declare read/write functions of the structure data (serialization)
#define __GDECLARE_POD_SERIALIZATION_FUNCTIONS(baseType)																										\
	uint32_t	fileDeserializeData		(GREF(baseType)** out_lstDataInstances		, uint32_t in_nInstanceCount, FILE* in_fp);									\
	uint32_t	fileSerializeData		(GREF(baseType)* const* in_lstDataInstances	, uint32_t in_nInstanceCount, FILE* out_fp);								\
	uint32_t	memDeserializeData		(GREF(baseType)** out_lstDataInstances		, uint32_t in_nInstanceCount, const void* in_pMemoryBuffer);				\
	uint32_t	memSerializeData		(GREF(baseType)* const* in_lstDataInstances	, uint32_t in_nInstanceCount, void* out_pMemoryBuffer);

#define __GDECLARE_POD_STREAMING_FUNCTIONS(baseType)																											\
	uint32_t	fileReadData			(GREF(baseType)** out_lstDataInstances		, uint32_t in_nInstanceCount, FILE* in_fp);									\
	uint32_t	fileWriteData			(GREF(baseType)* const* in_lstDataInstances	, uint32_t in_nInstanceCount, FILE* out_fp, const baseType* DefaultData);	\
	uint32_t	memReadData				(GREF(baseType)** out_lstDataInstances		, uint32_t in_nInstanceCount, const void* in_pMemoryBuffer);				\
	uint32_t	memWriteData			(GREF(baseType)* const* in_lstDataInstances	, uint32_t in_nInstanceCount, void* out_pMemoryBuffer, const baseType* DefaultData);

// This macro is used to declare all core functions at once
#define __GDECLARE_POD_FUNCTIONS(baseType)					\
	__GDECLARE_POD_MEMORY_FUNCTIONS				(baseType);	\
	__GDECLARE_POD_STREAMING_FUNCTIONS			(baseType);	\
	__GDECLARE_POD_SERIALIZATION_FUNCTIONS		(baseType);	\
	__GDECLARE_COMMON_DEBUG_STRING_FUNCTIONS	(baseType);	\
	static inline	void	onCreateDefault##baseType	(baseType* stuff)	{ *stuff = {}; }

#define GDECLARE_POD(baseType, ...)			\
	GDECLARE_REF(baseType, __VA_ARGS__);	\
	__GDECLARE_POD_FUNCTIONS(baseType);

#define NWOL_POD(nameSpace, baseType, displayName, descriptionText, ...)												\
	NWOL_REGISTER_POD(nameSpace, baseType, displayName, descriptionText);												\
	GDECLARE_REF(baseType, __VA_ARGS__);																				\
	__GDECLARE_POD_FUNCTIONS(baseType);

#define GODS_POD_BEGIN(_structName)					struct _structName {
#define GODS_POD_MEMBER(_typeName, _memberName)			_typeName					_memberName
#define GODS_POD_END(_structName)					};

#define GODS_OBJ_BEGIN(_structName)					struct _structName {
#define GODS_OBJ_MEMBER(_typeName, _memberName)			_typeName					_memberName
#define GODS_OBJ_END(_structName)					};

#define GODS_NCO_BEGIN(_structName)					struct _structName {
#define GODS_NCO_MEMBER(_typeName, _memberName)			_typeName					_memberName
#define GODS_NCO_END(_structName)					};

#define GODS_PURE_BEGIN(_structName)				struct _structName {
#define GODS_PURE_MEMBER(_typeName, _memberName)		_typeName					_memberName
#define GODS_PURE_END(_structName)					};

namespace nwol
{
	template<typename _tRef>	static inline	_tRef*			acquire			(_tRef* instanceRef)								{ return instanceRef ? instanceRef->acquire() : nullptr; }
	template<typename _tRef>	static inline	void			release			(_tRef** instanceRef)								{
		typedef		void			(*TFunctionRelease)	(_tRef**);
		_tRef															* pRef			= *instanceRef;
		if(pRef) {
			*instanceRef												= 0;
			((TFunctionRelease)pRef->Globals->_prelease)(&pRef);
		}
	}
	template<typename _tRef>	static inline	void			set				(_tRef** out_Ref, _tRef* in_Ref)					{
		_tRef															* old			= *out_Ref;																
		*out_Ref													= ::nwol::acquire(in_Ref);															
		::nwol::release(&old);																		
	}																																	
	template<typename _tRef>	static inline	void			release			(_tRef** instanceRefs, uint32_t* instanceCount)		{
		for(uint32_t i = 0, count = *instanceCount; i < count; ++i)			
			::nwol::release(&instanceRefs[i]);							
	}
	template<typename _tRef>	static inline	int32_t			compare			(const _tRef* in_A, const _tRef* in_B)				{
		if (0 == in_A && 0 == in_B)																									
			return 0;																												
		else if (0 == in_A || 0 == in_B)																							
			return 0x7FFFFFFF;																										
																																	
		return ::nwol::podcmp(in_A->Instance, in_B->Instance);																		
	}
} // namespace
#endif // GREF_H_2398429385492835498234
