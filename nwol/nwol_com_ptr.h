#include "nwol_log.h"
#include <Unknwn.h>

#ifndef NWOL_COM_PTR_H_0928364982374
#define NWOL_COM_PTR_H_0928364982374 

namespace nwol 
{
	template<typename _tCOM>
	class com_ptr {
		typedef				com_ptr<_tCOM>					TCOMPtr;
#if defined(__WINDOWS__)
							_tCOM*							Instance							= 0;
#endif
	public:
		inline												~com_ptr							()									noexcept	{ if(Instance) Instance->Release();								}
		inline constexpr									com_ptr								()									noexcept	= default;
		inline												com_ptr								(const TCOMPtr& other)				noexcept	{ Instance = other.Instance; if(Instance) Instance->AddRef();	}
		inline constexpr									com_ptr								(TCOMPtr&& other)					noexcept	{ Instance = other.Instance; other.Instance = 0;				}
		inline												com_ptr								(_tCOM* other)						noexcept	{ Instance = other; if(Instance) Instance->AddRef();			}

		inline constexpr	operator						_tCOM*								()									noexcept	{ return Instance; }

		inline constexpr	TCOMPtr							operator =							(const TCOMPtr& other)				noexcept	{ _tCOM* oldInstance = Instance; Instance = other.Instance; Instance->AddRef(); if(oldInstance) oldInstance->Release(); return *this; }
		inline constexpr	TCOMPtr							operator =							(TCOMPtr&& other)					noexcept	{ _tCOM* oldInstance = Instance; Instance = other.Instance; other.Instance = 0; if(oldInstance) oldInstance->Release(); return *this; }
		inline constexpr	TCOMPtr							operator =							(_tCOM* other)						noexcept	{ _tCOM* oldInstance = Instance; Instance = other;								if(oldInstance) oldInstance->Release(); return *this; }

		inline				_tCOM*							operator->							()									noexcept	{ return Instance;	}
		inline				_tCOM**							operator &							()									noexcept	{ return &Instance;	}

		template<typename _tCOMOther>
		inline				::nwol::error_t					as									(_tCOMOther** other)				noexcept	{ nwol_hrcall(Instance->QueryInterface(__uuidof(_tCOMOther), (void**)other)); return 0; }
		inline				_tCOM*							acquire								()									noexcept	{ if(Instance) Instance->AddRef(); return Instance; }

		_Check_return_		::nwol::error_t					CoCreateInstance					(_In_ const IID& rclsid, _Inout_opt_ LPUNKNOWN pUnkOuter = NULL, _In_ DWORD dwClsContext = CLSCTX_ALL)		{
			_tCOM													* oldInstance						= Instance;
			nwol_hrcall(::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(_tCOM), (void**)&Instance));
			if(oldInstance)
				oldInstance->Release();
			return 0;
		}
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
		_Check_return_		::nwol::error_t					CoCreateInstance					(_In_z_ LPCOLESTR szProgID, _Inout_opt_ LPUNKNOWN pUnkOuter = NULL, _In_ DWORD dwClsContext = CLSCTX_ALL)	{
			CLSID													clsid								= {};
			nwol_hrcall(::CLSIDFromProgID(szProgID, &clsid));
			nwol_hrcall(::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(_tCOM), (void**)&p));
			return hr;
		}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	};
}

#endif // NWOL_COM_PTR_H_0928364982374