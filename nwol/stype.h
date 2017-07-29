/// Copyright 2015-2017 - asm128
#include "datatype.h"
#include "datausage.h"
#include "nwol_error.h"
#include <cstdio>

#ifndef NWOL_TYPE_H_23984729384
#define NWOL_TYPE_H_23984729384

namespace nwol
{
#pragma pack( push, 1 )
	// This type was intended to wrap native types and other basic PODs but now it's also used for most type of objects.
	template <typename _tBase,::nwol:: GDATA_TYPE _F, ::nwol::GDATA_USAGE _USAGE = ::nwol::GUSAGE_NUMERIC> 
	struct supertype {
		// Typedef so we don't have to write it all the time.
		typedef						supertype<_tBase, _F, _USAGE>	stype;
		typedef						_tBase							TBase;

									_tBase							Value						= {};

		inline														supertype					()												= default;
		inline														supertype					(const stype&		other)						= default;
		inline														supertype					(const _tBase&		other)						: Value(other)					{}

		inline														operator	_tBase&			()									noexcept	{ return Value;					}
		inline constexpr											operator	const _tBase&	()							const	noexcept	{ return Value;					}

		inline						_tBase*							operator	&				()									noexcept	{ return &Value;				}
		inline constexpr			const _tBase*					operator	&				()							const	noexcept	{ return &Value;				}

		inline						stype&							operator	=				(const _tBase& other)				noexcept	{ Value = other; return *this;	}
//		inline						stype&							operator	=				(const _tBase&& other)				noexcept	{ Value = other; return *this;	}

		// Static for lightweight type info retrieval
		static inline constexpr		::nwol::SDataTypeID				get_data_type_id			()									noexcept	{ return _F;					}
		static inline constexpr		::nwol::GDATA_USAGE				get_data_usage				()									noexcept	{ return _USAGE;				}
	};

	template <typename _tBase> struct spointer : public supertype<_tBase*, ::nwol::GDATA_TYPE_POINTER, ::nwol::GUSAGE_POINTER>
	{
		typedef						supertype<_tBase, ::nwol::GDATA_TYPE_POINTER, ::nwol::GUSAGE_POINTER>	stype;

		using						stype::							supertype;
		inline														spointer					()												: stype::supertype((_tBase*)0)	{}

		inline						_tBase*							operator->					()									noexcept	{ return this->Value;			}
		inline						const _tBase*					operator->					()							const	noexcept	{ return this->Value;			}
	};	

	template <typename _tBase, ::nwol::GDATA_TYPE _F, ::nwol::GDATA_USAGE _USAGE = ::nwol::GUSAGE_NUMERIC> struct spod : public supertype<_tBase, _F, _USAGE>
	{
		typedef						supertype<_tBase, _F, _USAGE>	stype;

		using						stype::							supertype;
		typedef						spod<_tBase, _F, _USAGE>		Tspod;

		inline						_tBase*							operator->					()									noexcept	{ return  &this->Value;				}
		inline constexpr			const _tBase*					operator->					()							const	noexcept	{ return  &this->Value;				}
		inline						_tBase*							begin						()									noexcept	{ return  &this->Value;				}
		inline constexpr			const _tBase*					begin						()							const	noexcept	{ return  &this->Value;				}
		inline						_tBase*							end							()									noexcept	{ return (&this->Value)+1;			}
		inline constexpr			const _tBase*					end							()							const	noexcept	{ return (&this->Value)+1;			}

		// These are wrappers for memcpy() and fwrite()/fread() intended to reduce program verbosity. Disk operations return an error while memory operations return the size copied and zero if failed to write.
		inline						uint32_t						read						(const void* fp)								{ retnul_error_if(0 == fp, "Parameter is null!");	this->Value = *reinterpret_cast<const _tBase*>(fp);	return (uint32_t)sizeof(_tBase); }
		inline						uint32_t						write						(void* fp)					const				{ retnul_error_if(0 == fp, "Parameter is null!");	*reinterpret_cast<_tBase*>(fp) = this->Value;		return (uint32_t)sizeof(_tBase); }

		inline						::nwol::error_t					read						(FILE* fp)										{ reterr_error_if(0 == fp, "Cannot read file from a null pointer.");	return (1 == fread (&this->Value, sizeof(_tBase), 1, fp)) ? 0 : -1; }
		inline						::nwol::error_t					write						(FILE* fp)					const				{ reterr_error_if(0 == fp, "Cannot write to a null pointer to file.");	return (1 == fwrite(&this->Value, sizeof(_tBase), 1, fp)) ? 0 : -1; }
	};

	// signed
	typedef spod<double		, ::nwol::GDATA_TYPE_FLOAT64	, ::nwol::GUSAGE_SCALAR>	sfloat64	, sdouble		;
	typedef spod<float		, ::nwol::GDATA_TYPE_FLOAT32	, ::nwol::GUSAGE_SCALAR>	sfloat32	, sfloat		;
	typedef spod<int64_t	, ::nwol::GDATA_TYPE_INT64		, ::nwol::GUSAGE_SCALAR>	sint64		, slonglong		;
	typedef spod<int32_t	, ::nwol::GDATA_TYPE_INT32		, ::nwol::GUSAGE_SCALAR>	sint32		, sint			, slong		;
	typedef spod<int16_t	, ::nwol::GDATA_TYPE_INT16		, ::nwol::GUSAGE_SCALAR>	sint16		, sshort		;
	typedef spod<int8_t		, ::nwol::GDATA_TYPE_INT8		, ::nwol::GUSAGE_SCALAR>	sint8		, schar			;

	// unsigned
	typedef spod<uint64_t	, ::nwol::GDATA_TYPE_UINT64		, ::nwol::GUSAGE_SCALAR>	suint64		, sulonglong	;
	typedef spod<uint32_t	, ::nwol::GDATA_TYPE_UINT32		, ::nwol::GUSAGE_SCALAR>	suint32		, suint			, sulong	;
	typedef spod<uint16_t	, ::nwol::GDATA_TYPE_UINT16		, ::nwol::GUSAGE_SCALAR>	suint16		, sushort		;
	typedef spod<uint8_t	, ::nwol::GDATA_TYPE_UINT8		, ::nwol::GUSAGE_SCALAR>	suint8		, suchar		;

	// bool?
	typedef spod<bool		, ::nwol::GDATA_TYPE_UINT8		, ::nwol::GUSAGE_BOOLEAN>	sbool;

	typedef spod<id_t		, ::nwol::GDATA_TYPE_INT32		, ::nwol::GUSAGE_INDEX>		sid32;
	typedef spod<int64_t	, ::nwol::GDATA_TYPE_INT64		, ::nwol::GUSAGE_INDEX>		sid64;
#pragma pack( pop )

};

#endif // NWOL_TYPE_H_23984729384
