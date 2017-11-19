#include "nwol_member_registry.h"
#include "nwol_mutex.h"
#include "nwol_runtime.h"
#include "nwol_ascii_target.h"

#include "gui.h"
#include "nwol_input.h"
#include "netlib_node.h"
//#include "netlib_server.h"

#ifndef NWOL_APPLICATION_H_98364902634__
#define NWOL_APPLICATION_H_98364902634__

namespace gddm
{
	struct SRendererDetail {
#if defined(__WINDOWS__)	
		
#endif
	};

#pragma pack(push, 1)
	struct SAlpha {
		bool				EnableTest			: 1;
		bool				EnableBlend			: 1;
		uint8_t				BlendValue			;
		uint32_t			ColorRef			; 
		// BLENDFACTOR
		// SRCBLENDALPHA
		// DESTBLENDALPHA
		// BLENDOPALPHA
		// ALPHAFUNC
	};

	struct SFog {
		bool				Enable				: 1;
		bool				Color				: 1;
		bool				TableMode			: 1;
		bool				Start				: 1;
		bool				End					: 1;
		bool				RangeFogEnable		: 1;
		uint8_t				Density				;
		uint8_t				VertexMode			;
	};

	struct SStencil {
		bool				Enable				: 1;
		bool				Fail				: 1;
		bool				ZFail				: 1;
		bool				Pass				: 1;
		bool				Func				: 1;
		uint8_t				Ref					;
		uint8_t				Mask				;
		uint8_t				WriteMask			;
	};

	struct SRaster {
		uint8_t				ZEnable				: 1;
		uint8_t				ScissorTestEnable   : 1;
		uint8_t				ZWriteEnable		: 1;
		uint8_t				FillMode			: 2;
		uint8_t				CullMode			: 2;
		uint8_t				ZFunc				: 4;
	};
#pragma pack(pop)

	struct SRenderer {
		SFog				FogState			= {};
		SAlpha				AlphaState			= {};
		SStencil			Stencil				= {};
		SRaster				Raster				= {};
		// SHADEMODE                  
		// LASTPIXEL                  
		// SRCBLEND                   
		// DESTBLEND                  
		// DITHERENABLE               
		// TEXTUREFACTOR              
		// WRAP0                      
		// WRAP1                      
		// WRAP2                      
		// WRAP3                      
		// WRAP4                      
		// WRAP5                      
		// WRAP6                      
		// WRAP7                      
		// CLIPPING                   
		// LIGHTING                   
		// AMBIENT                    
		// COLORVERTEX                
		// LOCALVIEWER                
		// NORMALIZENORMALS           
		// DIFFUSEMATERIALSOURCE      
		// SPECULARMATERIALSOURCE     
		// AMBIENTMATERIALSOURCE      
		// EMISSIVEMATERIALSOURCE     
		// VERTEXBLEND                
		// CLIPPLANEENABLE            
		// POINTSIZE                  
		// POINTSIZE_MIN              
		// POINTSPRITEENABLE          
		// POINTSCALEENABLE           
		// POINTSCALE_A               
		// POINTSCALE_B               
		// POINTSCALE_C               
		// MULTISAMPLEANTIALIAS       
		// MULTISAMPLEMASK            
		// PATCHEDGESTYLE             
		// DEBUGMONITORTOKEN          
		// POINTSIZE_MAX              
		// INDEXEDVERTEXBLENDENABLE   
		// COLORWRITEENABLE           
		// TWEENFACTOR                
		// BLENDOP                    
		// POSITIONDEGREE             
		// NORMALDEGREE               
		// SLOPESCALEDEPTHBIAS        
		// ANTIALIASEDLINEENABLE      
		// MINTESSELLATIONLEVEL       
		// MAXTESSELLATIONLEVEL       
		// ADAPTIVETESS_X             
		// ADAPTIVETESS_Y             
		// ADAPTIVETESS_Z             
		// ADAPTIVETESS_W             
		// ENABLEADAPTIVETESSELLATION 
		// TWOSIDEDSTENCILMODE        
		// CCW_STENCILFAIL            
		// CCW_STENCILZFAIL           
		// CCW_STENCILPASS            
		// CCW_STENCILFUNC            
		// COLORWRITEENABLE1          
		// COLORWRITEENABLE2          
		// COLORWRITEENABLE3          
		// SRGBWRITEENABLE            
		// DEPTHBIAS                  
		// WRAP8                      
		// WRAP9                      
		// WRAP10                     
		// WRAP11                     
		// WRAP12                     
		// WRAP13                     
		// WRAP14                     
		// WRAP15                     
		// SEPARATEALPHABLENDENABLE   
	};


	GDEFINE_ENUM_TYPE	(NETWORK_STATE, uint8_t);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, NONE	, 0);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, ENABLED	, 1);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, RUNNING	, 2);

	struct SFrameworkNetworkClient {
		NWOM(::gddm, SFrameworkNetworkClient, ::gdnet, SNetworkNode	,	Connection				, ::nwol::GDATA_TYPE_OBJ		, "Connection link"				, "Holds socket and connection information."								)	= {};
		NWOM(::gddm, SFrameworkNetworkClient, ::nwol, CMutex		,	ServerTimeMutex			, ::nwol::GDATA_TYPE_OBJ		, "Server time mutex"			, "Locks access to server time variable."									)	= {};
		NWOM(::gddm, SFrameworkNetworkClient, , uint64_t			,	ServerTime				, ::nwol::GDATA_TYPE_UINT64		, "Server time"					, "Holds server time."														)	= 0;
		NWOM(::gddm, SFrameworkNetworkClient, ::gddm, NETWORK_STATE,	State					, ::nwol::GDATA_TYPE_UINT8		, "Network state bitfield"		, "Holds netowrk and connection states."									)	= NETWORK_STATE_NONE;
		NWOM_REGISTRY
			(	NWOM_NAME(Connection		)
			,	NWOM_NAME(ServerTimeMutex	)
			,	NWOM_NAME(ServerTime		)
			,	NWOM_NAME(State				)
			);
	};	// struct

	struct SFramework {
		NWOM(::gddm, SFramework, ::nwol, SRuntimeValues*			,	RuntimeValues			, ::nwol::GDATA_TYPE_POINTER	, "Runtime values"				, "Stores runtime states and loaded modules."								)	= nullptr;
		NWOM(::gddm, SFramework, ::nwol, SASCIITarget				,	ASCIITarget				, ::nwol::GDATA_TYPE_OBJ		, "ASCII Render target"			, "Backbuffer used for ascii console rendering."							)	= {};
		NWOM(::gddm, SFramework, ::nwol, SInput						,	Input					, ::nwol::GDATA_TYPE_OBJ		, "Input subsystem"				, "Holds information of user input events and states."						)	= {};
		NWOM(::gddm, SFramework, ::nwol, SDisplayInput				,	MainScreenInput			, ::nwol::GDATA_TYPE_OBJ		, "Main screen input subsystem"	, "Holds information of user input events and states for a given screen."	)	= {};
		NWOM(::gddm, SFramework, ::nwol, SGUI						,	GUI						, ::nwol::GDATA_TYPE_OBJ		, "Graphical user interface"	, "GUI control information is stored and drawn by this object."				)	= {{640, 480}, {132, 60},};
		NWOM(::gddm, SFramework, ::gddm, SRenderer					,	Renderer				, ::nwol::GDATA_TYPE_OBJ		, "Render device"				, "Used to render graphics to the backbuffer."								)	= {};
		NWOM(::gddm, SFramework, ::gddm, SFrameworkNetworkClient	,	NetworkClient			, ::nwol::GDATA_TYPE_OBJ		, "Network client"				, "Holds client connection state and server information."					)	= {};	// 
		NWOM(::gddm, SFramework, ::nwol, CMutex						,	ConsoleMutex			, ::nwol::GDATA_TYPE_OBJ		, "Console mutex"				, "Locks access to backbuffer required for multithreaded rendering."		)	= {};
		NWOM_REGISTRY
			(	NWOM_NAME(RuntimeValues		)
			,	NWOM_NAME(Input				)
			,	NWOM_NAME(MainScreenInput	)
			,	NWOM_NAME(GUI				)
			,	NWOM_NAME(Renderer			)
			,	NWOM_NAME(NetworkClient		)
			,	NWOM_NAME(ConsoleMutex		)
			);

#if defined(__ANDROID__)
		inline																SFramework				(::nwol::SRuntimeValues	* runtimeValues)						noexcept	: RuntimeValues(runtimeValues)									{
			runtimeValues->AndroidAppState->userData							= this;					
			runtimeValues->AndroidAppState->onAppCmd							= SFramework::handleAppCommand;	
		}

		static	void														handleAppCommand		(android_app * androidAppState, int32_t androidCommand)			{}
#else
		inline																SFramework				(::nwol::SRuntimeValues	* runtimeValues)						noexcept	: RuntimeValues(runtimeValues)									{}
#endif
	};	// struct

}	// namespace

#endif // NWOL_APPLICATION_H_98364902634__

