#ifndef __GREF_DEFINITION_PURE_H__209387423980742__
#define __GREF_DEFINITION_PURE_H__209387423980742__

#define __GDEFINE_PURE_RELEASE( baseType )																		\
	void grelease( GREF(baseType)** _p )																		\
	{																											\
		if( 0 == (*_p) )																						\
			return;																								\
																												\
		typedef void	(*funcType)		(GREF(baseType)**);														\
		if( (*_p)->Globals->__prelease != (funcType)grelease )													\
			((funcType)(*_p)->Globals->__prelease)(_p);															\
		else																									\
			error_printf("Unresolved deallocator for type: '%s'.", GREF(baseType)::get_type_name().begin());	\
	}

#define GDEFINE_PURE_FUNCTIONS_NONS(structName)	\
	__GDEFINE_PURE_RELEASE(structName)

#endif // __GREF_DEFINITION_PURE_H__209387423980742__