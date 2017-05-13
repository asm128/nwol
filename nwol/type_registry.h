#include "type_identifier.h"
#include "nwol_array.h"
#include "datatype.h"


#ifndef __TYPE_REGISTRY_H__92874982742__
#define __TYPE_REGISTRY_H__92874982742__

namespace nwol
{
	struct STypeMetrics {
								int32_t													Size							;
								int32_t													SizePadded						;
								int32_t													Align							;
								int32_t													AlignMultiplier					;

		constexpr				bool													operator==						(const STypeMetrics& other)		const	noexcept	{	
			return	Size			== 	other.Size			
				&&	SizePadded		== 	other.SizePadded		
				&&	Align			== 	other.Align			
				&&	AlignMultiplier	== 	other.AlignMultiplier	
				;
		}
									
		// This function takes an argument that informs of the padding size used by the system, which in general ranges from 4 to 32 in powers of 2.
		template<typename _tBase>	
								void													set								(const uint32_t paddingBlockSize)		noexcept	{
			Size																			= ::nwol::get_type_size				<_tBase>();
			SizePadded																		= ::nwol::get_type_size_padded		<_tBase>(paddingBlockSize);
			Align																			= ::nwol::get_type_align			<_tBase>();
			AlignMultiplier																	= ::nwol::get_type_align_multiplier	<_tBase>();	
		}
	};	// struct

	class STypeRegistry {
		// Holds indices to name arrays.
		struct STypeRecord {
									int32_t													Namespace						= -1;
									int32_t													Type							= -1;
									int32_t													DisplayName						= -1;
									int32_t													Description						= -1;
									int32_t													DataTypeId						= -1;
									int32_t													TypeMetrics						= -1;
//									int32_t													Identifier						= -1;
		};
								::nwol::array_pod<STypeRecord					>		Records							= {};
								::nwol::array_pod<::nwol::STypeIdentifier		>		Types							= {};	
								::nwol::array_pod<::nwol::gsyslabel				>		Namespaces						= {};
								::nwol::array_pod<::nwol::gsyslabel				>		TypeNames						= {};
								::nwol::array_pod<::nwol::glabel				>		DisplayNames					= {};
								::nwol::array_pod<::nwol::glabel				>		Descriptions					= {};
								::nwol::array_pod<::nwol::GDATA_TYPE			>		DataTypeIds						= {};
								::nwol::array_pod<::nwol::STypeMetrics			>		TypeMetrics						= {};
	public:
		static					STypeRegistry&											get								()										noexcept	{ static STypeRegistry			registryInstance;	return registryInstance			; }
		static inline			const uint32_t											get_record_count				()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.Records.size()	; }
		static inline			const ::nwol::array_pod<::nwol::STypeIdentifier	>&		get_identifiers					()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.Types			; }
		static inline			const ::nwol::array_pod<::nwol::gsyslabel		>&		get_namespaces					()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.Namespaces		; }
		static inline			const ::nwol::array_pod<::nwol::gsyslabel		>&		get_type_names					()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.TypeNames		; }
		static inline			const ::nwol::array_pod<::nwol::glabel			>&		get_display_names				()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.DisplayNames	; }
		static inline			const ::nwol::array_pod<::nwol::glabel			>&		get_descriptions				()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.Descriptions	; }
		static inline			const ::nwol::array_pod<::nwol::GDATA_TYPE		>&		get_data_type_ids				()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.DataTypeIds		; }
		static inline			const ::nwol::array_pod<::nwol::STypeMetrics	>&		get_type_metrics				()										noexcept	{ static const STypeRegistry&	instance = get();	return instance.TypeMetrics		; }

		template<typename _tBase>
								::nwol::error_t											register_pod
			(	const ::nwol::STypeIdentifier	& identifier
			,	const ::nwol::glabel			& name_display		
			,	const ::nwol::glabel			& description		
			,	const ::nwol::GDATA_TYPE		& data_type_id
			)
		{
			for(uint32_t i=0; i<Types.size(); ++i) {
				if(identifier == Types[i]) {
					warning_printf("Type already registered: %s::%s", identifier.NameSpace.begin(), identifier.Name.begin());
					return 1;
				}
			}
			STypeRecord																			record;
			::nwol::STypeMetrics																typeSizes;
			typeSizes.set<_tBase>(BASETYPE_ALIGN);
			info_printf("Registering type: %s::%s. Size: %u. Size Padded: %u. Align: %u. Align multiplier: %u.", identifier.NameSpace.begin(), identifier.Name.begin(), typeSizes.Size, typeSizes.SizePadded, typeSizes.Align, typeSizes.AlignMultiplier);

			record.Namespace	= Namespaces	.find(identifier.NameSpace	); if(-1 == record.Namespace	) { record.Namespace	= Namespaces	.push_back(identifier.NameSpace	); throw_if(record.Namespace	== -1, "", "Failed to push \"%s\" to type registry record.", "Namespace"	) }
			record.Type			= TypeNames		.find(identifier.Name		); if(-1 == record.Type			) { record.Type			= TypeNames		.push_back(identifier.Name		); throw_if(record.Type			== -1, "", "Failed to push \"%s\" to type registry record.", "Type"			) }
			record.DisplayName	= DisplayNames	.find(name_display			); if(-1 == record.DisplayName	) { record.DisplayName	= DisplayNames	.push_back(name_display			); throw_if(record.DisplayName	== -1, "", "Failed to push \"%s\" to type registry record.", "DisplayName"	) }
			record.Description	= Descriptions	.find(description			); if(-1 == record.Description	) { record.Description	= Descriptions	.push_back(description			); throw_if(record.Description	== -1, "", "Failed to push \"%s\" to type registry record.", "Description"	) }
			record.DataTypeId	= DataTypeIds	.find(data_type_id			); if(-1 == record.DataTypeId	) { record.DataTypeId	= DataTypeIds	.push_back(data_type_id			); throw_if(record.DataTypeId	== -1, "", "Failed to push \"%s\" to type registry record.", "DataTypeId"	) }
			record.TypeMetrics	= TypeMetrics	.find(typeSizes				); if(-1 == record.TypeMetrics	) { record.TypeMetrics	= TypeMetrics	.push_back(typeSizes			); throw_if(record.TypeMetrics	== -1, "", "Failed to push \"%s\" to type registry record.", "TypeMetrics"	) }
			throw_if(-1 == Types	.push_back(identifier	), "", "Failed to push \"%s\" to type registry.",  "identifier"	);
			throw_if(-1 == Records	.push_back(record		), "", "Failed to push \"%s\" to type registry.",  "record"		);
			return 0;
		}

	}; // struct

	template<typename _tBase>
	struct SPODRecordInitializer {
								int32_t													Value;

		inline					operator												int32_t							()								const	noexcept	{ return Value; }

																						SPODRecordInitializer
			(	const ::nwol::STypeIdentifier	& identifier		
			,	const ::nwol::glabel			& display_name
			,	const ::nwol::glabel			& description	
			,	::nwol::GDATA_TYPE				data_type_id
			) 
		{
			static	const int32_t																didntInitialize					= ::nwol::STypeRegistry::get().register_pod<_tBase>(identifier, display_name, description, data_type_id);																			
			Value																			= didntInitialize;
		}
	};	// struct

	template<typename _tBase>
	struct SOBJRecordInitializer {
								int32_t													Value;

		inline					operator												int32_t							()								const	noexcept	{ return Value; }

																						SOBJRecordInitializer
			(	const ::nwol::STypeIdentifier	& identifier		
			,	const ::nwol::glabel			& display_name
			,	const ::nwol::glabel			& description	
			,	::nwol::GDATA_TYPE				data_type_id
			) 
		{
			static	const int32_t																didntInitialize					= ::nwol::STypeRegistry::get().register_pod<_tBase>(identifier, display_name, description, data_type_id);																			
			Value																			= didntInitialize;
		}
	};	// struct

	template<typename _tBase>
	struct SNCORecordInitializer {
								int32_t													Value;

		inline					operator												int32_t							()								const	noexcept	{ return Value; }

																						SNCORecordInitializer
			(	const ::nwol::STypeIdentifier	& identifier		
			,	const ::nwol::glabel			& display_name
			,	const ::nwol::glabel			& description	
			,	::nwol::GDATA_TYPE				data_type_id
			) 
		{
			static	const int32_t																didntInitialize					= ::nwol::STypeRegistry::get().register_pod<_tBase>(identifier, display_name, description, data_type_id);																			
			Value																			= didntInitialize;
		}
	};	// struct

	template<typename _tBase>
	struct SMODRecordInitializer {
								int32_t													Value;

		inline					operator												int32_t							()								const	noexcept	{ return Value; }

																						SMODRecordInitializer
			(	const ::nwol::STypeIdentifier	& identifier		
			,	const ::nwol::glabel			& display_name
			,	const ::nwol::glabel			& description	
			,	::nwol::GDATA_TYPE				data_type_id
			) 
		{
			static	const int32_t																didntInitialize					= ::nwol::STypeRegistry::get().register_pod<_tBase>(identifier, display_name, description, data_type_id);																			
			Value																			= didntInitialize;
		}
	};	// struct

#define NWOL_REGISTER_TYPE(_initializerType, _dataTypeId, _nameSpace, _baseType, _displayName, _descriptionText)	static	const char	__nworeg_##_baseType##_		= (char)(int32_t)_initializerType<_nameSpace::_baseType>({#_nameSpace, #_baseType}, _displayName, _descriptionText, _dataTypeId)
#define NWOL_REGISTER_POD(_nameSpace, _baseType, _displayName, _descriptionText)									NWOL_REGISTER_TYPE (::nwol::SPODRecordInitializer, ::nwol::GDATA_TYPE_POD, _nameSpace, _baseType, _displayName, _descriptionText)
#define NWOL_REGISTER_OBJ(_nameSpace, _baseType, _displayName, _descriptionText)									NWOL_REGISTER_TYPE (::nwol::SOBJRecordInitializer, ::nwol::GDATA_TYPE_OBJ, _nameSpace, _baseType, _displayName, _descriptionText)
#define NWOL_REGISTER_NCO(_nameSpace, _baseType, _displayName, _descriptionText)									NWOL_REGISTER_TYPE (::nwol::SNCORecordInitializer, ::nwol::GDATA_TYPE_NCO, _nameSpace, _baseType, _displayName, _descriptionText)
#define NWOL_REGISTER_MOD(_nameSpace, _baseType, _displayName, _descriptionText)									NWOL_REGISTER_TYPE (::nwol::SMODRecordInitializer, ::nwol::GDATA_TYPE_MOD, _nameSpace, _baseType, _displayName, _descriptionText)
} // namespace

#endif // __TYPE_REGISTRY_H__92874982742__
