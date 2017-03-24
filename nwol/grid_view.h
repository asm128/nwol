#include "array_view.h"

#ifndef __GRID_VIEW_H__926340923746098743230948__
#define __GRID_VIEW_H__926340923746098743230948__

namespace nwol
{
#pragma pack(push, 1)
	template<typename _T>
	class grid_view
	{
	protected:
		_T*												Data			= nullptr;
		uint32_t										Width			= 0;
		uint32_t										Height			= 0;
	public:

		static				const uint32_t				element_size	= (uint32_t)sizeof(_T);
		//
		inline constexpr								grid_view		()														noexcept	= default;
		inline constexpr								grid_view		(const grid_view& other)								noexcept	= default;
		inline constexpr								grid_view		(_T* Data, uint32_t width, uint32_t height)				noexcept	: Data(Data), Width	((uint32_t)width), Height((uint32_t)height)		{}
																																	
		template <size_t _Width, size_t _Height>																					
		inline constexpr								grid_view		(const _T (&Data)[_Height][_Width])						noexcept	: Data(Data), Width	((uint32_t)_Width), Height((uint32_t)_Height)	{}
																																
		template <size_t _Width, size_t _Height>																				
		inline constexpr								grid_view		(_T	(&Data)[_Height][_Width])							noexcept	: Data(Data), Width	((uint32_t)_Width), Height((uint32_t)_Height)	{}

		// Report error before dereferencing invalid index.
		inline				grid_view<_T>&				operator=		(const grid_view& other)								noexcept	= default;
		inline				array_view<const _T>		operator[]		(uint32_t row)									const	noexcept	{ 
			if(row >= Height) 
				error_printf("%s: %u", "Invalid grid row", row); 
			return array_view<const _T>(&Data[row*this->Width], this->Width);
		}

		// Report error before dereferencing invalid index.
		inline				array_view<_T>				operator[]		(uint32_t row)											noexcept	{ 
			if(row >= Height) 
				error_printf("%s: %u", "Invalid grid row", row); 
			return array_view<_T>(&Data[row*this->Width], this->Width);
		}

		inline constexpr	uint32_t					width			()												const	noexcept	{ return Width;					}
		inline constexpr	uint32_t					height			()												const	noexcept	{ return Height;				}

		//inline constexpr	bool						empty			()												const	noexcept	{ return 0 == Count;			}
		inline constexpr	uint32_t					size			()												const	noexcept	{ return Width * Height;		}
		inline				_T*							begin			()														noexcept	{ return Data;					}
		inline constexpr	const _T*					begin			()												const	noexcept	{ return Data;					}
		inline				 _T*						end				()														noexcept	{ return Data + size();			}
		inline constexpr	const _T*					end				()												const	noexcept	{ return Data + size();			}
	};
#pragma pack(pop)
}

#endif // __GRID_VIEW_H__926340923746098743230948__

