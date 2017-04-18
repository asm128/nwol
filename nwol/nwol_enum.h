#include "label.h"
#include "nwol_array.h"

#ifndef __ENUM_H__982364987234987234__
#define __ENUM_H__982364987234987234__

namespace nwol
{
	static				const ::nwol::gsyslabel			INVALID_ENUM_VALUE_STR			= "INVALID";
	//static				const ::nwol::gsyslabel			UNDEFINED_ENUM_TYPE_STR		= "Undefined enumeration type.";
	static				const ::nwol::gsyslabel			UNDEFINED_ENUM_VALUE_STR		= "Undefined enumeration value.";
	static				const ::nwol::gsyslabel			UNRESOLVED_ENUM_LABEL_STR		= "Unresolved enumeration value name.";

	// This template is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	template <typename _tValue>
	struct genum_definition {
							::nwol::gsyslabel				Name;
							::nwol::array_pod<_tValue>		Values;
							::nwol::array_pod<gsyslabel>	Names;

		inline												genum_definition				()																	: Name("Enum definition name not set.")						{}
		inline static		genum_definition<_tValue>&		get								()																	{
			static	genum_definition<_tValue>						valueRegistry;
			return valueRegistry;
		}
		inline static		_tValue							init							(const ::nwol::gsyslabel& enumName)									{
					genum_definition<_tValue>&						instanceHere					= get();

			if( instanceHere.Name != enumName || (instanceHere.Values.size() && (instanceHere.Values[0] != ((_tValue)~(_tValue)0))) )
				info_printf("Initializing enumeration type: '%s'.", enumName.begin());

			static	::nwol::gsyslabel								newName							= instanceHere.Name = enumName;
			static	::nwol::error_t									errDummy						= instanceHere.add_value((_tValue)(~(_tValue)0), INVALID_ENUM_VALUE_STR);

			return (_tValue)~(_tValue)0;
		}
							::nwol::error_t					get_value						(const ::nwol::gsyslabel& name, _tValue& value)			const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i]) {
					value												= Values[i];
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			value = (_tValue)~(_tValue)0;
			return -1;
		}
							_tValue							get_value						(const ::nwol::gsyslabel& name)						const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i])
					return Values[i];

			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return (_tValue)~(_tValue)0;
		}
							::nwol::error_t					get_value_by_index				(uint32_t index, _tValue& value)							const			{
			if( index < Values.size() ) {
				value												= Values[index];
				return 0;
			}
			else {
				error_printf("Enumeration index out of range! Index: 0x%u.", index);
				value = (_tValue)~(_tValue)0;
				return -1;
			}
		}
							_tValue							get_value_by_index				(uint32_t index)									const			{
			retval_msg_if((_tValue)~(_tValue)0, index >= Values.size(), "Enumeration index out of range! Index: 0x%u.", index)
			return Values[index];
		}
							::nwol::error_t					get_label_by_index				(uint32_t index, ::nwol::gsyslabel& value)			const			{
			if( index < Names.size() ) {
				value												= Names[index];
				return 0;
			}
			else {
				value												= UNDEFINED_ENUM_VALUE_STR;
				error_printf("Enumeration index out of range! Index: 0x%u.", index);
				return -1;
			}
		}
							::nwol::gsyslabel				get_label_by_index				(uint32_t index)									const			{
			if( index < Names.size() ) {
				return Names[index];
			}
			else {
				error_printf("Enumeration index out of range! Index: 0x%u.", index);
				return UNDEFINED_ENUM_VALUE_STR;
			}
		}
							::nwol::error_t					get_value_index					(const ::nwol::gsyslabel& name, int32_t& index)		const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i]) {
					index												= (int32_t)i;
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			index = -1;
			return -1;
		}
							int32_t							get_value_index					(const ::nwol::gsyslabel& name)						const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i])
					return (int32_t)i;

			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return -1;
		}
							::nwol::error_t					get_value_index					(const _tValue& value, int32_t& index)					const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(value == Values[i]) {
					index												= (int32_t)i;
					return 0;
				}
			error_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			index = -1;
			return -1;
		}
							int32_t							get_value_index					(const _tValue& value)									const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(value == Values[i])
					return (int32_t)i;

			error_printf("Enumeration value not found! Value: %llX.", (uint64_t)value);
			return -1;
		}
							::nwol::error_t					get_value_label					(const _tValue& value, ::nwol::gsyslabel& name)			const			{
			for(uint32_t i=0, count = Values.size(); i<count; ++i)
				if(value == Values[i]) {
					name												= Names[i];
					return 0;
				}
			error_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name = UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
							::nwol::gsyslabel				get_value_label					(const _tValue& value)									const			{
			for(uint32_t i=0, count = Values.size(); i<count; ++i)
				if(value == Values[i])
					return Names[i];

			error_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return UNRESOLVED_ENUM_LABEL_STR;
		}
							::nwol::error_t					add_value						(const _tValue& value, const ::nwol::gsyslabel& name)					{
			for(uint32_t i=0, count = Values.size(); i<count; ++i)
				if(Values[i] == value) {
					if(Names[i] != name) {
						warning_printf("Enumeration value already defined! Type: '%s'. Value: 0x%llX. Previous name: %s. New name: %s. Second definition ignored..."
							, Name				.begin()
							, (uint64_t)value
							, Names[i]			.begin()
							, name				.begin()
							);
						return 1;
					}
					return 0;	// Found same name and value combination. This is normal when values are defined as static const.
				}
			const uint32_t											oldSize							= Values.size();
			uint32_t												newIndex						= Values.push_back(value);
			reterr_error_if(newIndex != oldSize, "Failed to add value to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());

			uint32_t												newNameIndex					= Names.push_back(name);
			if(newNameIndex != newIndex) {
				Values.resize(oldSize);
				reterr_error_if(true, "Failed to add value name to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			}
			verbose_printf("Added new value to enumeration definition. Enum name: %s. Index: %.02u, Value: 0x%llX. Value name: %s."
				, Name.begin()
				, (uint32_t)newIndex
				, (uint64_t)value
				, name.begin()
				);
			return 0;
		}
	};

	// This type is used to initialize an enumeration value.
	template <typename _tValue>
	struct genum_value {
							_tValue							Value							= (_tValue)~(_tValue)0;
							::nwol::gsyslabel				Name							= INVALID_ENUM_VALUE_STR;
		// 
		inline												genum_value						()																	= default;
		inline constexpr									genum_value						(const genum_value& other)											= default;
		inline												genum_value						(const _tValue& value)												: Value((_tValue)value), Name(::nwol::genum_definition<_tValue>::get().get_value_name(value))	{}
		inline constexpr									genum_value						(const _tValue& value, const ::nwol::gsyslabel& name)				: Value((_tValue)value), Name(name)														{ 
			::nwol::genum_definition<_tValue>::get().add_value(value, name); 
		}
		inline constexpr	operator						const	_tValue&				()														const		{ return Value; }
	};

	template <typename _tEnum>	::nwol::gsyslabel		get_value_label					(const _tEnum& statusBit				)								{ return ::nwol::genum_definition<_tEnum>::get().get_value_label	(statusBit);	}
	template <typename _tEnum>	int32_t					get_value_index					(const _tEnum& statusBit				)								{ return ::nwol::genum_definition<_tEnum>::get().get_value_index	(statusBit);	}
	template <typename _tEnum>	_tEnum					get_value						(const ::nwol::gsyslabel& valueLabel	)								{ return ::nwol::genum_definition<_tEnum>::get().get_value			(valueLabel);	}
} // namespace

// Defines the enumeration type, the invalid value (-1) and the flag operators 
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																																								\
	enum EnumName : IntType {};																																												\
	static					const ::nwol::gsyslabel		EnumName##_STR					= #EnumName;																										\
	static					const EnumName				EnumName##_INVALID				= (EnumName)::nwol::genum_definition<EnumName>::init(EnumName##_STR);												\
	static inline			EnumName					operator &						(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a & (IntType)b);				}	\
	static inline			EnumName					operator ~						(EnumName  a)								noexcept	{ return (EnumName)		(~(IntType)a);					}	\
	static inline			EnumName					operator ^						(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a ^ (IntType)b);				}	\
	static inline			EnumName&					operator |=						(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) |= (IntType)b); }	\
	static inline			EnumName&					operator &=						(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) &= (IntType)b); }	\
	static inline			EnumName&					operator ^=						(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) ^= (IntType)b); }	\
	static inline constexpr	EnumName					operator |						(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a | (IntType)b);					}

#define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)																												\
	static constexpr		const EnumName				EnumName##_##ValueName			= (EnumName)(EnumValue);														\
	static					const EnumName				__sei_##EnumName##_##ValueName	= (EnumName)::nwol::genum_value<EnumName>((EnumName)(EnumValue), #ValueName)

#define GDEFINE_ENUM_VALUE_NOPREFIX(EnumName, ValueName, EnumValue)																										\
	static constexpr		const EnumName				ValueName						= (EnumName)(EnumValue);														\
	static					const EnumName				__sei_##EnumName##_##ValueName	= (EnumName)::nwol::genum_value<EnumName>((EnumName)(EnumValue), #ValueName)

#define GDEFINE_FLAG_TYPE GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE GDEFINE_ENUM_VALUE
#define GDEFINE_FLAG_VALUE_NOPREFIX GDEFINE_ENUM_VALUE_NOPREFIX

#pragma warning(disable : 4063)

#endif // __ENUM_H__982364987234987234__
