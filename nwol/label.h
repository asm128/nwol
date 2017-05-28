#include "array_view.h"
#include "nwol_error.h"

#include <string>

#ifndef __LABEL_H_61596841996481968498__
#define __LABEL_H_61596841996481968498__

namespace nwol
{
	class	CLabelManager;
#pragma pack(push, 1)
	struct	glabel_statics;
	class glabel : public array_view<const char> {
	protected:
		CLabelManager											* LabelManager																	= nullptr;
	public:
		//static				const ::nwol::glabel				empty_label;
		static				const ::nwol::glabel_statics&		statics					();

																glabel					(const char* label, uint32_t size)						; // <---- this is the only constructor that actually matters.
		inline constexpr										glabel					()														= default;
		inline constexpr										glabel					(const ::nwol::glabel& other)							= default;
		inline													glabel					(const ::std::string& stdString)						: glabel(stdString.c_str(), (uint32_t)stdString.size())						{}
		template<size_t Size> 
		inline													glabel					(const char (&label)[Size])								: glabel(label, (uint32_t)Size)												{}

		inline				operator							::std::string			()									const				{ return ::std::string(begin());											}
		inline constexpr	operator							bool					()									const	noexcept	{ return (Count != 0);														}

		inline				::nwol::glabel&						operator=				(const ::nwol::glabel& other)				noexcept	= default;
							bool								operator==				(const ::nwol::glabel& other)		const	noexcept	;
		inline				bool								operator!=				(const ::nwol::glabel& other)		const	noexcept	{ return !operator==(other);												}

		inline constexpr	const char*							c_str					()									const	noexcept	{ return begin();															}

							uint32_t							save					(char* out_pMemoryBuffer)			const				;
							uint32_t							load					(const char* in_pMemoryBuffer)							;
							::nwol::error_t						save					(FILE* out_pMemoryBuffer)			const				;
							::nwol::error_t						load					(FILE* in_pMemoryBuffer)								;
	};

	// This label type is used to store strings for RTTI strings and other low level text. It has been added to separate user strings from system strings for improving nwol::glabel creation speed.
	// This is because when a new string is added to the pool, gunordered_string_set has to perform a search in order to verify the existence of the string in the set.
	// This way the strings are better organized for their given usage and prevents system strings from slowing down user string creation.
	struct gsyslabel_statics;
	class gsyslabel : public glabel {
	public:
																gsyslabel				(const char* label, uint32_t size);	// <------ this is the only constructor that actually matters.
		template<size_t Size> 
		inline constexpr										gsyslabel				(const char (&label)[Size])								: gsyslabel(label, (uint32_t)Size)											{}
		static				const ::nwol::gsyslabel_statics&	statics					();
	};

	// clang seems to be not conformant with calling template constructors, so we have to initialize them manually.
	struct gsyslabel_statics {
							gsyslabel							empty					= {""	, 1}											;
							gsyslabel							space					= {" "	, 1}											;
							gsyslabel							slash					= {"/"	, 1}											;
							gsyslabel							backslash				= {"\\"	, 1}											;
							gsyslabel							question_mark			= {"?"	, 1}											;
							gsyslabel							exclamation_mark		= {"!"	, 1}											;
							gsyslabel							dot						= {"."	, 1}											;
							gsyslabel							comma					= {","	, 1}											;
	};

	struct glabel_statics {
							glabel								empty					= gsyslabel::statics().empty							;
							glabel								space					= gsyslabel::statics().space							;
							glabel								slash					= gsyslabel::statics().slash							;
							glabel								backslash				= gsyslabel::statics().backslash						;
							glabel								question_mark			= gsyslabel::statics().question_mark					;
							glabel								exclamation_mark		= gsyslabel::statics().exclamation_mark					;
							glabel								dot						= gsyslabel::statics().dot								;
							glabel								comma					= gsyslabel::statics().comma							;
	};

	inline		const ::nwol::glabel_statics	&	glabel::		statics					()													{ static const ::nwol::glabel_statics		init_statics; return init_statics;	}
	inline		const ::nwol::gsyslabel_statics	&	gsyslabel::		statics					()													{ static const ::nwol::gsyslabel_statics	init_statics; return init_statics;	}
#pragma pack(pop)
};

#endif // __LABEL_H_61596841996481968498__