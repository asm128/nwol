#ifndef PLATFORM_HANDLE_WRAPPER_H_023984023984023894
#define PLATFORM_HANDLE_WRAPPER_H_023984023984023894

namespace nwol 
{
	// --------------------------------------------- 
	template<typename _tHandle>
	struct platform_handle_wrapper {
							_tHandle					Handle						= 0;

		inline constexpr								platform_handle_wrapper		()													noexcept	= default;

		inline constexpr								platform_handle_wrapper		(const _tHandle& other)								noexcept	= delete;
		inline constexpr								platform_handle_wrapper		(const platform_handle_wrapper<_tHandle>& other)	noexcept	= delete;
		inline constexpr	platform_handle_wrapper		operator=					(const _tHandle& other)								noexcept	= delete;

		inline constexpr	operator					const _tHandle&				()										const		noexcept	{ return Handle;		}
		inline constexpr	_tHandle*					operator&					()													noexcept	{ return &Handle;		}
		inline constexpr	const _tHandle*				operator&					()										const		noexcept	{ return &Handle;		}
	};
} // namespace

#endif // PLATFORM_HANDLE_WRAPPER_H_023984023984023894