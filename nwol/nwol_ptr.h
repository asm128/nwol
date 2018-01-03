#include "nwol_log.h"
#include "nwol_sync.h"
#include "nwol_memory.h"

#ifndef NWOL_PTR_H_293748098237423
#define NWOL_PTR_H_293748098237423

namespace nwol 
{
	template<typename _tInstance>
	struct nwol_ref {
		typedef		_tInstance								TInstance;

					_tInstance								* Instance;
					refcount_t								References;
	};
	
	template<typename _tNCO>
						::nwol::nwol_ref<_tNCO> *		ref_acquire						(::nwol::nwol_ref<_tNCO>* nwol_reference)												{
		if(nwol_reference)
			nwol_sync_increment(nwol_reference->References);
		return nwol_reference;
	};

	template<typename _tNCO>
						::nwol::error_t					ref_release						(::nwol::nwol_ref<_tNCO>** nwol_reference)											{
		typedef	::nwol::nwol_ref<_tNCO>							TRef;
		TRef													* oldRef							= *nwol_reference;
		*nwol_reference										= 0;
		if(oldRef)
			switch(nwol_sync_decrement(oldRef->References)) {
			case -1: error_printf("Reference count error!"); return -1;
			case  0:
				if(oldRef->Instance)
					oldRef->Instance->~_tNCO();
				else
					error_printf("Instance is NULL! At the point of writing this code it wouldn't make any sense.");
				::nwol::nwol_free(oldRef->Instance);
				::nwol::nwol_free(oldRef);
				break;
			}
		return 0;
	};

	template<typename _tOBJ>
						_tOBJ *							ref_allocate						(::nwol::nwol_ref<_tOBJ>** nwol_reference)								{
		typedef	::nwol::nwol_ref<_tOBJ>							TRef;
		TRef													* newRef							= (TRef*)::nwol::nwol_malloc(sizeof(TRef));
		retnul_error_if(0 == newRef, "Failed to allocate reference! Out of memory?");
		if(0 == (newRef->Instance = (_tOBJ*)::nwol::nwol_malloc(sizeof(_tOBJ)))) {
			::nwol::nwol_free(newRef);
			error_printf("Failed to allocate instance! Out of memory?");
			return 0;
		}
		newRef->References									= 1;
		TRef													* oldRef							= *nwol_reference;
		*nwol_reference										= newRef;
		::nwol::ref_release(&oldRef);
		return (*nwol_reference)->Instance;
	};

	template<typename _tOBJ, typename... _tArgs>
						_tOBJ *							ref_create							(::nwol::nwol_ref<_tOBJ>** nwol_reference, _tArgs&&... argsConstructor)	{
		typedef	::nwol::nwol_ref<_tOBJ>							TRef;
		TRef													* newRef							= 0;
		retnul_error_if(0 == ::nwol::ref_allocate(&newRef), "Failed to allocate reference");
		new (newRef->Instance) _tOBJ{argsConstructor...};

		TRef													* oldRef							= *nwol_reference;
		*nwol_reference										= newRef;
		::nwol::ref_release(&oldRef);
		return (*nwol_reference)->Instance;
	};

	template<typename _tNCO>
	class ptr_nco {
	protected:
							::nwol::nwol_ref<_tNCO>			* Instance							= 0;
	public:
		typedef				::nwol::ptr_nco<_tNCO>			TNCOPtr;
		typedef				::nwol::nwol_ref<_tNCO>			TRef;

		inline												~ptr_nco							()													noexcept	{ ::nwol::ref_release(&Instance);												}
		inline constexpr									ptr_nco								()													noexcept	= default;
		inline												ptr_nco								(const TNCOPtr& other)								noexcept	{ Instance = ::nwol::ref_acquire(other.Instance);								}
		inline constexpr									ptr_nco								(TNCOPtr&& other)									noexcept	{ Instance = other.Instance; other.Instance = 0;								}
		inline												ptr_nco								(TRef* other)										noexcept	{ Instance = other;																}

		inline constexpr	operator						_tNCO	*							()													noexcept	{ return Instance ? Instance->Instance : 0;										}

		inline constexpr	bool							operator==							(const TNCOPtr& other)						const	noexcept	{ return Instance == other.Instance;											}
		inline constexpr	bool							operator!=							(const TNCOPtr& other)						const	noexcept	{ return !operator==(other);													}

		inline constexpr	TNCOPtr							operator =							(const TNCOPtr& other)								noexcept	{ TRef* oldInstance = Instance; Instance = ::nwol::ref_acquire(other.Instance);	::nwol::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr							operator =							(TNCOPtr&& other)									noexcept	{ TRef* oldInstance = Instance; Instance = other.Instance; other.Instance = 0;	::nwol::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr							operator =							(TRef* other)										noexcept	{ TRef* oldInstance = Instance; Instance = other;								::nwol::ref_release(&oldInstance); return *this; }

		inline				_tNCO*							operator->							()													noexcept	{ return Instance->Instance; }
		inline				const _tNCO*					operator->							()											const	noexcept	{ return Instance->Instance; }

		inline				TRef**							operator &							()													noexcept	{ return &Instance;	}

		inline constexpr	const TRef*						get_ref								()											const	noexcept	{ return Instance;	}

		template<typename _tNCOOther>
		inline				_tNCO*							as									(_tNCOOther** other)								noexcept	{ return *other = (Instance ? (_tNCOOther*)Instance->Instance : 0);				}

		template<typename _tNCOOther>
		inline				_tNCO*							as									(TNCOPtr& other)									noexcept	{ other = ::nwol::ref_acquire(Instance); return 0;								}
	};
	
	template<typename _tOBJ>
	class ptr_obj : public ::nwol::ptr_nco<_tOBJ> {
	public:
		typedef				::nwol::ptr_nco<_tOBJ>			TNCOPtr;
		typedef				::nwol::ptr_obj<_tOBJ>			TOBJPtr;
		typedef				::nwol::nwol_ref<_tOBJ>			TRef;
		using				TNCOPtr							::Instance;

		inline				const _tOBJ*					operator->							()											const	noexcept	{ return Instance;																}
		inline				_tOBJ*							operator->							()													noexcept	{ return (0 == Instance) ? ::nwol::ref_create(&Instance) : Instance->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tOBJ*							create								(_tArgsConstructor&&... argsConstructor)			noexcept	{ return ::nwol::ref_create(&Instance, argsConstructor...);						}
		inline				_tOBJ*							allocate							()													noexcept	{ return ::nwol::ref_allocate(&Instance);										}
	};

	template<typename _tPOD>
	class ptr_pod : public ::nwol::ptr_nco<_tPOD> {
	public:
		typedef				::nwol::ptr_nco<_tPOD>			TNCOPtr;
		typedef				::nwol::ptr_pod<_tPOD>			TPODPtr;
		typedef				::nwol::nwol_ref<_tPOD>			TRef;
		using				TNCOPtr							::Instance;

		inline				const _tPOD*					operator->							()											const	noexcept	{ return Instance->Instance;																}
		inline				_tPOD*							operator->							()													noexcept	{ return (0 == Instance) ? ::nwol::ref_allocate(&Instance) : Instance->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tPOD*							create								(_tArgsConstructor&&... argsConstructor)			noexcept	{ return ::nwol::ref_create(&Instance, argsConstructor...);									}
		inline				_tPOD*							allocate							()													noexcept	{ return ::nwol::ref_allocate(&Instance);													}
	};
}

#endif // NWOL_PTR_H_293748098237423
