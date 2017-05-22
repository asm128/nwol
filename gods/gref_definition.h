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
#ifndef __GREF_DEFINITION_H__20347892374__
#define __GREF_DEFINITION_H__20347892374__
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

#define GDEFINE_POD_COMMON(NameSpace, baseType)					\
	namespace NameSpace{										\
		void grelease(GREF(baseType)** _p);						\
	}															\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)		\
	namespace NameSpace{										\
		__GDEFINE_POD_FUNCTIONS(baseType);						\
	}

#define GDEFINE_POD_NONS_COMMON(baseType)			\
	void grelease(GREF(baseType)** _p);				\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\
	__GDEFINE_POD_FUNCTIONS(baseType);

#define GDEFINE_POD_CUSTOMSTRING(NameSpace, baseType, format, ...)	\
	GDEFINE_POD_COMMON(NameSpace, baseType)							\
	GDEFINE_PRINTPOD(NameSpace, baseType, format, __VA_ARGS__)

#define GDEFINE_POD_NONS_CUSTOMSTRING(baseType, format, ...)	\
	GDEFINE_POD_NONS_COMMON(baseType)							\
	GDEFINE_PRINTPOD_NONS(baseType, format, __VA_ARGS__)

#define GDEFINE_POD(NameSpace, baseType)		\
	GDEFINE_POD_COMMON(NameSpace, baseType)		\
	GDEFINE_PRINTPOD(NameSpace, baseType, "{%s}", "")

#define GDEFINE_POD_NONS(baseType)				\
	GDEFINE_POD_NONS_COMMON(baseType)			\
	GDEFINE_PRINTPOD_NONS(baseType, "{%s}", "")	

//-------------------------------------------------------------// GDEFINE_NCO() //------------------------------------------------------------------------// 
#define GDEFINE_NCO_FUNCTIONS_NONS(baseType)																						\
	static	::nwol::gref_manager_nco<GREF(baseType)>	__g_##baseType##Manager(grelease);											\
	/*static ::nwol::gref_manager_nco<GREF(baseType)>	__g_##baseType##Manager(::nwol::_nwol_internal_release<GREF(baseType)>);*/	\
	__GDEFINE_RELEASE(baseType);																						\
	__GDEFINE_ALLOC(baseType)

#define GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE(NameSpace, baseType)	\
	namespace NameSpace{											\
		void grelease(GREF(baseType)** _p);							\
	}																\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)			\
	namespace NameSpace{											\
		GDEFINE_NCO_FUNCTIONS_NONS(baseType)						\
	}

#define GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE_NONS(baseType)	\
	void grelease(GREF(baseType)** _p);						\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)			\
	GDEFINE_NCO_FUNCTIONS_NONS(baseType)

#define GDEFINE_NCO_CUSTOMSTRING(NameSpace, baseType, format, ...)	\
	GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE(NameSpace, baseType)		\
	GDEFINE_PRINTOBJ(NameSpace, baseType, format, __VA_ARGS__)	
	
#define GDEFINE_NCO_CUSTOMPAGE(NameSpace, baseType)				\
	GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE(NameSpace, baseType)	\
	GDEFINE_PRINTOBJ(NameSpace, baseType, "{%s}", "")

#define GDEFINE_NCO_CUSTOMPAGE_NONS(baseType, pageSize)				\
	GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE_NONS(baseType, pageSize)	\
	GDEFINE_PRINTOBJ_NONS(baseType, "{%s}", "")	

#define GDEFINE_NCO(NameSpace, baseType)						\
	GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE(NameSpace, baseType)	\
	GDEFINE_PRINTOBJ(NameSpace, baseType, "{%s}", "")

#define GDEFINE_NCO_NONS(baseType)						\
	GDEFINE_NCO_CUSTOMCLEANUP_CUSTOMPAGE_NONS(baseType)	\
	GDEFINE_PRINTOBJ_NONS(baseType, "{%s}", "")	

//-------------------------------------------------------------// GDEFINE_OBJ() //------------------------------------------------------------------------// 
#define GDEFINE_OBJ_FUNCTIONS_NONS(baseType)											\
	static ::nwol::gref_manager_obj<GREF(baseType)>	__g_##baseType##Manager(grelease);	\
	__GDEFINE_RELEASE(baseType)															\
	__GDEFINE_COPY_REF_FUNCTIONS(baseType);												\
	__GDEFINE_CREATEOBJECT(baseType);													\
	__GDEFINE_ALLOC(baseType);

#define GDEFINE_OBJ_CUSTOMPAGE(NameSpace, baseType)			\
	namespace NameSpace{									\
		void grelease(GREF(baseType)** _p);					\
	}														\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)	\
	namespace NameSpace {									\
		GDEFINE_OBJ_FUNCTIONS_NONS(baseType);				\
	}

#define GDEFINE_OBJ_CUSTOMPAGE_NONS(baseType)		\
	void grelease(GREF(baseType)** _p);				\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\
	GDEFINE_OBJ_FUNCTIONS_NONS(baseType)	

#define GDEFINE_OBJ_CUSTOMSTRING(NameSpace, baseType, format, ...)	\
	GDEFINE_OBJ_CUSTOMPAGE(NameSpace, baseType)						\
	GDEFINE_PRINTOBJ(NameSpace, baseType, format, __VA_ARGS__)

#define GDEFINE_OBJ_CUSTOMSTRING_NONS(baseType, format, ...)	\
	GDEFINE_OBJ_CUSTOMPAGE_NONS(baseType)						\
	GDEFINE_PRINTOBJ(baseType, format, __VA_ARGS__)	

#define GDEFINE_OBJ(NameSpace, baseType)				\
	GDEFINE_OBJ_CUSTOMPAGE(NameSpace, baseType)			\
	GDEFINE_PRINTOBJ(NameSpace, baseType, "{%s}", "")

#define GDEFINE_OBJ_NONS(baseType)			\
	GDEFINE_OBJ_CUSTOMPAGE_NONS(baseType)	\
	GDEFINE_PRINTOBJ_NONS(baseType, "{%s}", "")				

//-------------------------------------------------------------// GDEFINE_PURE() //------------------------------------------------------------------------// 
#define GDEFINE_PURE_NOPRINT(NameSpace, baseType)			\
	__GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)	\

#define GDEFINE_PURE_NOPRINT_NONS(baseType)			\
	__GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\

#define GDEFINE_PURE_CUSTOMSTRING(NameSpace, baseType, format, ...)		\
	GDEFINE_PURE_NOPRINT(NameSpace, baseType)							\
	GDEFINE_PRINTOBJ(NameSpace, baseType, format, __VA_ARGS__)

#define GDEFINE_PURE_CUSTOMSTRING_NONS(NameSpace, baseType, format, ...)	\
	GDEFINE_PURE_NOPRINT_NONS(NameSpace, baseType)							\
	GDEFINE_PRINTOBJ_NONS(NameSpace, baseType, format, __VA_ARGS__)	

#define GDEFINE_PURE(NameSpace, baseType)				\
	GDEFINE_PURE_NOPRINT(NameSpace, baseType)			\
	GDEFINE_PRINTOBJ(NameSpace, baseType, "{%s}", "")

#define GDEFINE_PURE_NONS(baseType)				\
	GDEFINE_PURE_NOPRINT_NONS(baseType)			\
	GDEFINE_PRINTOBJ_NONS(baseType, "{%s}", "")

// ---- garbage ----

static const char* STRING_BOOL_TRUE		= "true";
static const char* STRING_BOOL_FALSE	= "false";

inline static const char* G_CSTRING_FROM_BOOL(bool b) {
  return b ? STRING_BOOL_TRUE : STRING_BOOL_FALSE;
}

#endif // __GREF_DEFINITION_H__20347892374__
