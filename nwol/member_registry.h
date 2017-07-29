/// Copyright 2016-2017 - asm128
#include "type_identifier.h"
#include "datatype.h"

#ifndef NWOL_MEMBER_REGISTRY_H_9214982364982734
#define NWOL_MEMBER_REGISTRY_H_9214982364982734

#define NWOM_REGISTRY_NAME		nowl_member_registry
#define NWOM_NAME(_memberName)	_nwol_info_##_memberName##_	// This mangles the member name for use as the type name.

#define NWOM(_structNameSpace, _structName, _nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)																													\
	struct NWOM_NAME(_memberName)	{																																												\
		static inline constexpr	const char_t*												get_member_namespace			()							noexcept	{ return #_nameSpace;						}	\
		static inline constexpr	const char_t*												get_member_type_name			()							noexcept	{ return #_memberType;						}	\
		static inline constexpr	const char_t*												get_member_name					()							noexcept	{ return #_memberName;						}	\
		static inline constexpr	const char_t*												get_member_display_name			()							noexcept	{ return _displayName;						}	\
		static inline constexpr	const char_t*												get_member_description			()							noexcept	{ return _memberDescription;				}	\
		static inline constexpr	::nwol::GDATA_TYPE											get_member_data_type_id			()							noexcept	{ return _dataTypeId;						}	\
		static inline constexpr	uint32_t													get_member_size					()							noexcept	{ return sizeof(_nameSpace::_memberType);	}	\
	};																																																				\
	_nameSpace::_memberType		_memberName

namespace nwol {
#pragma pack(push, 1)
	template <typename... _tArgs>	struct member_registry {
		static inline constexpr	const uint32_t												get_member_count				()							noexcept	{ return (uint32_t)sizeof...(_tArgs);																																																															}
		static inline			const ::nwol::array_view<const ::nwol::STypeIdentifier	>&	get_types						()										{ static const ::nwol::STypeIdentifier	results[] = {{{			_tArgs::get_member_namespace(), (uint32_t)-1}, {_tArgs::get_member_type_name(), (uint32_t)-1}}...,	{}	}; static const ::nwol::array_view<const ::nwol::STypeIdentifier	> results_view = {results}; return results_view;	}
		static inline			const ::nwol::array_view<const ::nwol::gsyslabel		>&	get_names						()										{ static const ::nwol::gsyslabel		results[] = {{			_tArgs::get_member_name			(), (uint32_t)-1}	...,											""	}; static const ::nwol::array_view<const ::nwol::gsyslabel			> results_view = {results}; return results_view;	}
		static inline			const ::nwol::array_view<const ::nwol::glabel			>&	get_display_names				()										{ static const ::nwol::glabel			results[] = {{			_tArgs::get_member_display_name	(), (uint32_t)-1}	...,											""	}; static const ::nwol::array_view<const ::nwol::glabel				> results_view = {results}; return results_view;	}
		static inline			const ::nwol::array_view<const ::nwol::glabel			>&	get_descriptions				()										{ static const ::nwol::glabel			results[] = {{			_tArgs::get_member_description	(), (uint32_t)-1}	...,											""	}; static const ::nwol::array_view<const ::nwol::glabel				> results_view = {results}; return results_view;	}
		static inline constexpr	const ::nwol::array_view<const ::nwol::GDATA_TYPE		>&	get_data_type_ids				()							noexcept	{ static const ::nwol::GDATA_TYPE		results[] = {			_tArgs::get_member_data_type_id	()					...,					::nwol::GDATA_TYPE_UNKNOWN	}; static const ::nwol::array_view<const ::nwol::GDATA_TYPE			> results_view = {results}; return results_view;	}
		static inline constexpr	const ::nwol::array_view<const uint32_t					>&	get_member_sizes				()							noexcept	{ static const uint32_t					results[] = {(uint32_t)	_tArgs::get_member_size			()					...,											 0	}; static const ::nwol::array_view<const uint32_t					> results_view = {results}; return results_view;	}
	}; // struct
#	define NWOM_NAMED_REGISTRY(registryName, ...)	typedef ::nwol::member_registry<__VA_ARGS__> TRegistry;		static	const TRegistry&	get_member_registry	()	{ static const TRegistry registryName; return registryName;	}
#pragma pack(pop)
} // namespace 

#define	NWOMND(_memberType, _memberName)		NWOM(_memberType, _memberName, ::nwol::GDATA_TYPE_OBJECT, #_memberName, #_memberName)
#define NWOM_REGISTRY(...)						NWOM_NAMED_REGISTRY(NWOM_REGISTRY_NAME, __VA_ARGS__)
#define NWOM_GET_MEMBER_REGISTRY(objectType)	objectType::get_member_registry()

#endif // NWOL_MEMBER_REGISTRY_H_9214982364982734
