#include "label.h"
#include "descriptor.h"
#include "gcore_ref.h"
#include "nwol_memory.h"

#ifndef __GREF_PTR_H__23487928374923__
#define __GREF_PTR_H__23487928374923__

namespace nwol
{
	// ------------------------------------------------------------ gptr_nco (ex-gbase_ptr) ------------------------------------------------------------  
	// This is the shared_ptr-like object for non-copiable non-constructable objects.
#define GPtrNCO(baseType)			::nwol::gptr_nco<GREF(baseType)>				
#define GPNCO(NameSpace, baseType)	::nwol::gptr_nco<NameSpace::GREF(baseType)>	
	template <typename _TRef> class gptr_nco {
	protected:
		typedef					void							(*TFunctionRelease)		(_TRef**)																						;
		typedef					typename						_TRef::TBase			_tBase																							;
								_TRef													* InstanceRef																					= 0;

	public:
		virtual													~gptr_nco				()																								{ ::nwol::release(&InstanceRef);									}	// 
		inline constexpr										gptr_nco				()																								= default;
		inline constexpr										gptr_nco				(_TRef* instanceRef)																			: InstanceRef(instanceRef)											{}
		inline													gptr_nco				(const gptr_nco<_TRef>& other)																	: InstanceRef(::nwol::acquire(other.InstanceRef))					{}
		// virtual operators ---------------				----------------------------------------------------------------
		virtual					gptr_nco&						operator =				(_TRef* instanceRef)																			{
			if (instanceRef == InstanceRef) {
				::nwol::release(&instanceRef);
				return *this;
			}
			_set(instanceRef);
			return *this;
		}

		virtual					gptr_nco&						operator =				(const gptr_nco<_TRef>& other)																	{
			if(other.InstanceRef != InstanceRef)
				_set(other.acquire());
			return *this;
		}

		virtual					_tBase*							operator->				()																								{ return InstanceRef->get();									}
		inline constexpr		const _tBase*					operator->				()																			const				{ return InstanceRef->get();									}
								_TRef**							operator &				()																					noexcept	{ return &InstanceRef;											}
		inline constexpr		_TRef* const*					operator &				()																			const	noexcept	{ return &InstanceRef;											}
																operator _TRef*&		()																								{ return InstanceRef;											}
		// inline constexpr		operators
		inline constexpr										operator _TRef* const &	()																			const	noexcept	{ return InstanceRef;											}
		inline constexpr		 bool							operator!				()																			const	noexcept	{ return 0 == InstanceRef;										}
		inline constexpr		 bool							operator==				(const gptr_nco<_TRef>& other)												const	noexcept	{ return InstanceRef == other.InstanceRef;						}
		inline constexpr		 bool							operator!=				(const gptr_nco<_TRef>& other)												const	noexcept	{ return InstanceRef != other.InstanceRef;						}
		inline constexpr		 bool							operator==				(_TRef* const & other)														const	noexcept	{ return InstanceRef == other;									}
		inline constexpr		 bool							operator!=				(_TRef* const & other)														const	noexcept	{ return InstanceRef != other;									}

		// virtual methods -	--------------------			--------------------------------------------------------											--
		virtual					 void							free					()																								{ _set(0);														}
		virtual					 void							acquire					(_TRef* const & instanceRef)																	{ 
			if(instanceRef != InstanceRef)
				_set(instanceRef ? instanceRef->acquire() : nullptr);	
		}

		// inline methods
		inline					_tBase*							get_address				()																								{ return InstanceRef ? InstanceRef->get() : 0;		}
		inline					const _tBase*					get_address				()																			const				{ return InstanceRef ? InstanceRef->get() : 0;		}

		// inline constexpr		methods
		inline constexpr		bool							valid					()																			const	noexcept	{ return 0 != InstanceRef;											}
		inline constexpr		_TRef*							get_pointer				()																			const	noexcept	{ return InstanceRef;												}
		inline constexpr		bool							am_I_owning				()																			const	noexcept	{ return InstanceRef == 0	|| InstanceRef->ReferenceCount == 1;	}
		inline constexpr		bool							writable				()																			const	noexcept	{ return InstanceRef		&& InstanceRef->ReferenceCount == 1;	}
		inline constexpr		_TRef*							acquire					()																			const	noexcept	{ return InstanceRef		 ? InstanceRef->acquire() : nullptr;	}

		// static methods
		static inline 			const ::nwol::gsyslabel&		get_type_name			()																								{ return _TRef::get_type_name();									}
		static inline constexpr	uint32_t						get_type_size			()																					noexcept	{ return sizeof(_tBase);											}

	protected:
		// virtual protected
		virtual					void							_set					(_TRef* const & instanceRef)																	{
			_TRef														* old					= InstanceRef;
			InstanceRef												= instanceRef;
			::nwol::release(&old);
		}
	};

	// ------------------------------------------------------------ gptr_obj ------------------------------------------------------------  
	// This is the shared_ptr-like object for proper C++ objects that can be copied or initialized by the generic manager.
#define GPtrObj(baseType)			::nwol::gptr_obj<GREF(baseType)>				
#define GPObj(NameSpace, baseType)	::nwol::gptr_obj<NameSpace::GREF(baseType)>	
	template <typename _TRef> 
	class gptr_obj : public gptr_nco<_TRef>
	{
		using					gptr_nco<_TRef>::				InstanceRef;
	protected:
		typedef					typename						_TRef::TBase			_tBase																							;
		virtual					void							_create					()																								{ gcreate(&InstanceRef);								}
		virtual					void							_create					(const _tBase& other)																			{ gcreate(&InstanceRef, other);							}
	public:
		inline													gptr_obj				(const _tBase* initData)																		{ if (initData) gcreate(&InstanceRef, *initData);		}
		inline													gptr_obj				(const gptr_nco<_TRef>& other)																	: gptr_nco<_TRef>::gptr_nco(other)						{}
		inline													gptr_obj				(const _tBase& initData)																		{ gcreate(&InstanceRef, initData);						}
		inline constexpr										gptr_obj				()																								= default;
		inline constexpr										gptr_obj				(const int initData)																			{}
		inline constexpr										gptr_obj				(_TRef* instanceRef)																			: gptr_nco<_TRef>::gptr_nco(instanceRef)				{}
		// virtual methods
		virtual					void							create					()																								{ _create();											}
		virtual					void							create					(const _tBase& other)																			{ _create(other);										}
		// operators
		using					gptr_nco<_TRef>::				operator->;
		virtual					_tBase*							operator->				()																								{
			if(0 == InstanceRef)
				_create();
			return InstanceRef->get();
		}

		// Clone in-place
		virtual					void							unbind					()																								{ gclone(&InstanceRef, InstanceRef);					}
		// For cloning in-place, use unbind()
		virtual					void							clone					(gptr_obj<_TRef>& target)													const				{ 
			if (0 == InstanceRef) {															
				target.free();									
				return;													
			}				
			gptr_obj<_TRef>												_target;
			_target.create();
			gcopy(_target, InstanceRef);											
			target													= _target;
		}
	};

	// ------------------------------------------------------------ gptr_pod ------------------------------------------------------------  
	// This is the shared_ptr-like object for plain old data structs and works with the data with standards primitives such as malloc() and memcpy(). 
	// It also defines methods for storing and loading the data into a file or a memory address.
#define GPtrPOD(baseType)			::nwol::gptr_pod<GREF(baseType)>				
#define GPPOD(NameSpace, baseType)	::nwol::gptr_pod<NameSpace::GREF(baseType)>	
	template <typename _TRef> 
	class gptr_pod : public gptr_obj<_TRef>
	{
		using					gptr_nco<_TRef>::				InstanceRef;
	public:
		typedef					typename						_TRef::TBase			_tBase																							;

		using					gptr_obj<_TRef>::				gptr_obj;

		virtual					const ::nwol::gdescriptor		get_descriptor			()																			const				{ return _TRef::get_type_descriptor();									}
		virtual					int32_t							compare_data			(_TRef* other)																const				{ 
			if (0 == InstanceRef && 0 == other)																					
				return 0;																								
			else if (0 == InstanceRef || 0 == other)																			
				return 0x7FFFFFFF;																						
																														
			return ::nwol::podcmp(InstanceRef->get(), other->get()); // 
		}

		inline					::nwol::error_t					write					(FILE* out_fp, const _tBase* defaultData = 0)								const				{ return (fileWriteData			(&InstanceRef, 1, out_fp, defaultData	) != 1) ? -1 : 0; }
		inline					::nwol::error_t					read					(FILE* in_fp)																					{ return (fileReadData			(&InstanceRef, 1, in_fp				) != 1) ? -1 : 0; }
		inline					::nwol::error_t					serialize				(FILE* out_fp)																const				{ return (fileSerializeData		(&InstanceRef, 1, out_fp				) != 1) ? -1 : 0; }
		inline					::nwol::error_t					deserialize				(FILE* in_fp)																					{ return (fileDeserializeData	(&InstanceRef, 1, in_fp				) != 1) ? -1 : 0; }
		virtual					::nwol::error_t					serialize				(char* out_fp, uint32_t* out_nReadBytes)									const				{
			if (0 == out_fp)
				return -1;
			uint32_t													nReadBytes				= memSerializeData(&InstanceRef, 1, out_fp);
			if (out_nReadBytes)
				*out_nReadBytes										+= nReadBytes;
			return 0;
		}

		virtual					::nwol::error_t					deserialize				(const char* in_fp, uint32_t* out_nReadBytes)													{
			if (0 == in_fp)
				return -1;
			uint32_t													nReadBytes				= memDeserializeData(&InstanceRef, 1, in_fp);
			if (out_nReadBytes)
				*out_nReadBytes										+= nReadBytes;
			return 0;
		}

		virtual					::nwol::error_t					write					(char* out_fp, uint32_t* out_nReadBytes, const _tBase* defaultData = 0)		const				{
			if (0 == out_fp)
				return -1;

			static constexpr	const uint32_t							nWrittenBytes			= ::nwol::get_type_size<_tBase>();
			static constexpr	const char								bytes[nWrittenBytes]	= {};
			memcpy(out_fp, InstanceRef ? InstanceRef->get() : defaultData ? defaultData : (_tBase*)bytes, nWrittenBytes);
			if (out_nReadBytes)
				*out_nReadBytes											+= nWrittenBytes;
			return 0;
		}

		virtual					::nwol::error_t					read					(const char* in_fp, uint32_t* out_nReadBytes)													{
			if (0 == in_fp)
				return -1;

			::nwol::gptr_pod<_TRef>										newInstance;
			static constexpr	const uint32_t							nReadBytes				= ::nwol::get_type_size<_tBase>();
			newInstance.create();
			memcpy(newInstance.get_address(), in_fp, nReadBytes);
			if (out_nReadBytes)
				*out_nReadBytes											+= nReadBytes;
			return 0;
		}
	}; // gptr_pod

	// ------------------------------------------------------------ gptr_podx ------------------------------------------------------------  
	// This is a gptr_pod which allows callbacks to be set for notifying about pointer events.
	//
	// Events work as follows:
	// 1. When an instance is created by either create() or operator->(), onCreate() callback is called.
	//    - An exception being when the instance is created by the constructor, then no onUpdate() is called. This is so the pointer can be initialized with an initial instance before any callbacks are called.
	// 2. When the pointer is set to a new address, onUpdate() callback is called.
	//    - An exception being when the pointer is set by the constructor, then no onUpdate() is called. This is so the pointer can be initialized with an initial instance before any callbacks are called.
	// 3. When the pointer is released and set to nullptr, onFree() callback is called.
#define GPtrPODX(baseType)			::nwol::gptr_podx<GREF(baseType)>				
#define GPPODX(NameSpace, baseType)	::nwol::gptr_podx<NameSpace::GREF(baseType)>	

	template<typename _TRef> class gptr_podx : public gptr_pod<_TRef>
	{
		using					gptr_nco<_TRef>::				InstanceRef																												;
		using					gptr_nco<_TRef>::				valid																													;
	protected:
		typedef					typename						_TRef::TBase			_tBase																							;

																gptr_podx				(void (*_onCreate)(_tBase *), void(*_onUpdate)(_tBase *), void(*_onFree)(_tBase *), const gptr_nco<_TRef>& other) : onCreate(_onCreate), onUpdate(_onUpdate), onFree(_onFree) { InstanceRef = other.acquire(); }
		virtual					void							_create					()																								{
			_TRef														* newInstance			= 0;
			gcreate(&newInstance);
			_set(newInstance);
			if (valid() && onCreate)
				(*onCreate)(InstanceRef->get());
		}

		virtual					void							_set					(_TRef* const & CoreInstance)																	{
			gptr_podx<_TRef>											old						(0, 0, onFree, m_History);	// call destroy handlers for history contents and release the instance.
			m_History												= InstanceRef;
			InstanceRef												= CoreInstance;
		}

	public:
								gptr_pod<_TRef>					m_History																												= {};	// history doesn't call update handlers.
								void							(*onCreate)				(_tBase* instance)																				= 0;
								void							(*onUpdate)				(_tBase* instance)																				= 0;
								void							(*onFree)				(_tBase* instance)																				= 0;

		virtual													~gptr_podx				()																								{
			if (m_History)
				free_history();
			if (valid() && onFree)
				(*onFree)(InstanceRef->get());
		}

		inline constexpr 										gptr_podx				()																												= default;
		inline													gptr_podx				(const gptr_nco<_TRef>& other)																					: gptr_podx()																{ InstanceRef = ::nwol::acquire(other.InstanceRef);	}
		inline													gptr_podx				(const gptr_podx<_TRef>& other)																					: onCreate(other.onCreate), onUpdate(other.onUpdate), onFree(other.onFree)	{ InstanceRef = ::nwol::acquire(other.InstanceRef);	}
		inline													gptr_podx				(_TRef* CoreInstance)																							: gptr_podx()																{ InstanceRef = CoreInstance;							}
		inline													gptr_podx				(const _tBase* initData)																						: onCreate(0), onUpdate(0), onFree(0)										{ if (initData)	gcreate(&InstanceRef, *initData);		}
		inline													gptr_podx				(const _tBase& initData)																						: onCreate(0), onUpdate(0), onFree(0)										{ gcreate(&InstanceRef, initData);						}
		inline													gptr_podx				(void(*_onCreate)(_tBase*), void(*_onUpdate)(_tBase*), void(*_onFree)(_tBase*), _TRef* other = 0)				: onCreate(_onCreate), onUpdate(_onUpdate), onFree(_onFree)					{ InstanceRef = other;									}
		inline													gptr_podx				(void(*_onCreate)(_tBase*), void(*_onUpdate)(_tBase*), void(*_onFree)(_tBase*), const gptr_pod<_TRef>& other)	: onCreate(_onCreate), onUpdate(_onUpdate), onFree(_onFree)					{ InstanceRef = other.acquire();						}
		// operators -----------------------------------	----------------	---------------------------------
		virtual					gptr_podx<_TRef>&				operator =				(const gptr_nco<_TRef>& other)																	{
			if( (0 == compare_data(other)) && 0 == m_History.compare_data(other) )
				return *this;

			_set(other.acquire());
			if (valid() && onUpdate)
				onUpdate(InstanceRef->get());

			return *this;
		}

		virtual					gptr_podx<_TRef>&				operator =				(const gptr_podx<_TRef>& other)																	{ return operator=((const gptr_pod<_TRef>&) other); }
		virtual					gptr_podx<_TRef>&				operator =				(_TRef* instanceRef)																			{
			if( (0 == compare_data(instanceRef)) && 0 == m_History.compare_data(instanceRef) ) {
				gptr_podx<_TRef>(0, 0, onFree, instanceRef);	// Catch the reference.
				return *this;
			}
			_set(instanceRef);
			if (valid() && onUpdate)
				(*onUpdate)(InstanceRef->get());
			return *this;
		}
		
		// methods ---------------------------------------------------------------------------------------
		virtual					void							free					()																								{
			if (valid() && onFree)
				(*onFree)(InstanceRef->get());
			_set(0);
		}

		inline					void							onCreateHandler			(void(*handler)(_tBase*))															noexcept	{ onCreate	= handler;	}
		inline					void							onUpdateHandler			(void(*handler)(_tBase*))															noexcept	{ onUpdate	= handler;	}
		inline					void							onFreeHandler			(void(*handler)(_tBase*))															noexcept	{ onFree	= handler;	}
		inline constexpr		const gptr_nco<_TRef>&			get_history				()																			const	noexcept	{ return m_History;		}
								void							roll_back				()																								{	
			// Swap the pointers.
			_TRef														* oldInstance			= InstanceRef;
			InstanceRef												= m_History.acquire();
			m_History												= oldInstance; // catch the reference
			if (valid() && onUpdate)
				(*onUpdate)(InstanceRef->get());
		}

								void							free_history			()																								{
			gptr_podx<_TRef>											old						(0, 0, onFree, m_History);	// call destroy handlers for history contents and release the instance.
			m_History.free();
		}
	};

}// namespace

#endif // __GREF_PTR_H__23487928374923__
