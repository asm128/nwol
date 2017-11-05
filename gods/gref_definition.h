/// gref_definition.h
///
/// Function definitions as macros for common functions for handling reference counting and save/load core structures to/from file/memory streams.
/// NOTES: 
/// - As this function contains function definitions, it should be include only in the .cpp files that require it, 
/// instead of including in a header file, in order to reduce compile times.
///
/// - These functions are low level functions written in C which were been tested thorougly and may require 
/// a good understanding of software engineering and resource management to be able to modify them properly.
/// 
/// This file is part of the Game Object Data System.
/// 2013 (c) Pablo Ariel Zorrilla Cepeda
#ifndef NWOL_GREF_DEFINITION_H_20347892374
#define NWOL_GREF_DEFINITION_H_20347892374

#include "gcore_ref.h"
#include "gref_serializer_pod.h"
#include "gref_manager.h"

#include "gref_definition_common.h"

//-------------------------------------------------------------// GDEFINE_POD() //------------------------------------------------------------------------// 
#define __GDEFINE_POD_FUNCTIONS_BUT_CREATEDATA_AND_INITTYPE(baseType)					\
	static ::nwol::gref_manager_pod<GREF(baseType)>	__g_##baseType##Manager(grelease);	\
	static ::nwol::gref_serializer_pod<GREF(baseType)>	__g_##baseType##Serializer;		\
	__GDEFINE_RELEASE(baseType);														\
	__GDEFINE_COPY_REF_FUNCTIONS(baseType);												\
	__GDEFINE_FILEMEM_SERIALIZATION_FUNCTIONS(baseType);								\
	__GDEFINE_FILEMEM_STREAMING_FUNCTIONS(baseType);

#define __GDEFINE_POD_FUNCTIONS(baseType)								\
		__GDEFINE_POD_FUNCTIONS_BUT_CREATEDATA_AND_INITTYPE(baseType)	\
		__GDEFINE_CREATEDATA(baseType);	

#define GDEFINE_POD(NameSpace, baseType)					\
	namespace NameSpace{										\
		void grelease(GREF(baseType)** _p);						\
	}															\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)		\
	namespace NameSpace{										\
		__GDEFINE_POD_FUNCTIONS(baseType);						\
	}

#define GDEFINE_POD_NONS(baseType)			\
	void grelease(GREF(baseType)** _p);				\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\
	__GDEFINE_POD_FUNCTIONS(baseType);

//-------------------------------------------------------------// GDEFINE_NCO() //------------------------------------------------------------------------// 
#define GDEFINE_NCO_FUNCTIONS_NONS(baseType)																						\
	static	::nwol::gref_manager_nco<GREF(baseType)>	__g_##baseType##Manager(grelease);											\
	/*static ::nwol::gref_manager_nco<GREF(baseType)>	__g_##baseType##Manager(::nwol::_nwol_internal_release<GREF(baseType)>);*/	\
	__GDEFINE_RELEASE(baseType);																						\
	__GDEFINE_ALLOC(baseType)

#define GDEFINE_NCO(NameSpace, baseType)	\
	namespace NameSpace{											\
		void grelease(GREF(baseType)** _p);							\
	}																\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)			\
	namespace NameSpace{											\
		GDEFINE_NCO_FUNCTIONS_NONS(baseType)						\
	}

#define GDEFINE_NCO_NONS(baseType)	\
	void grelease(GREF(baseType)** _p);						\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)			\
	GDEFINE_NCO_FUNCTIONS_NONS(baseType)

//-------------------------------------------------------------// GDEFINE_OBJ() //------------------------------------------------------------------------// 
#define GDEFINE_OBJ_FUNCTIONS_NONS(baseType)											\
	static ::nwol::gref_manager_obj<GREF(baseType)>	__g_##baseType##Manager(grelease);	\
	__GDEFINE_RELEASE(baseType)															\
	__GDEFINE_COPY_REF_FUNCTIONS(baseType);												\
	__GDEFINE_CREATEOBJECT(baseType);													\
	__GDEFINE_ALLOC(baseType);

#define GDEFINE_OBJ(NameSpace, baseType)			\
	namespace NameSpace{									\
		void grelease(GREF(baseType)** _p);					\
	}														\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)	\
	namespace NameSpace {									\
		GDEFINE_OBJ_FUNCTIONS_NONS(baseType);				\
	}

#define GDEFINE_OBJ_NONS(baseType)		\
	void grelease(GREF(baseType)** _p);				\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\
	GDEFINE_OBJ_FUNCTIONS_NONS(baseType)	

//-------------------------------------------------------------// GDEFINE_PURE() //------------------------------------------------------------------------// 
#define GDEFINE_PURE(NameSpace, baseType)	__GCORE_REF_INIT_GCORE_REF_MEMBERS(NameSpace, baseType)	
#define GDEFINE_PURE_NONS(baseType)			__GCORE_REF_INIT_GCORE_REF_MEMBERS_NONS(baseType)	

#endif // NWOL_GREF_DEFINITION_H_20347892374
