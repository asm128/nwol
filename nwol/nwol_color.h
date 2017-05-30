/// Copyright 2009-2017 - asm128
#include "nwol_math.h"

#ifndef __COLOR_H__29734982734__
#define __COLOR_H__29734982734__

namespace nwol
{

#pragma pack( push, 1 )
	// Stores BGRA color channels
	struct SColor32 {
							uint8_t			b = 0, g = 0, r = 0, a = 0xff; 

		inline constexpr					SColor32		()																noexcept	= default;
		inline constexpr					SColor32		(const SColor32& other)											noexcept	= default;
		inline constexpr					SColor32		(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_=0xff)			noexcept	: b(b_), g(g_), r(r_), a(a_)																																										{}
		inline constexpr					SColor32		(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0))), g((uint8_t)(((other & 0x0000FF00) >> 8))), r((uint8_t)(((other & 0x00FF0000) >> 16))), a((uint8_t)(((other & 0xFF000000) >> 24)))						{}

		inline constexpr	operator		uint32_t		()														const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																								}
		inline				SColor32&		operator=		(const SColor32& color)											noexcept	= default;
		inline constexpr	bool			operator ==		(uint32_t other)										const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		inline constexpr	bool			operator ==		(const SColor32& other)									const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		inline constexpr	bool			operator !=		(const SColor32& other)									const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		inline constexpr	SColor32		operator *		(const SColor32& color)									const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r * (uint16_t)color.r, 0, 255)	, (uint8_t)::nwol::clamp(g * (uint16_t)color.g, 0, 255)	,(uint8_t)::nwol::clamp(b * (uint16_t)color.b, 0, 255), a);			}
		inline constexpr	SColor32		operator +		(const SColor32& color)									const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r + (uint16_t)color.r, 0, 255)	, (uint8_t)::nwol::clamp(g + (uint16_t)color.g, 0, 255)	,(uint8_t)::nwol::clamp(b + (uint16_t)color.b, 0, 255), a);			}
		inline constexpr	SColor32		operator *		(float scalar)											const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r * scalar, 0.0f, 255.0f)		, (uint8_t)::nwol::clamp(g * scalar, 0.0f, 255.0f)		,(uint8_t)::nwol::clamp(b * scalar, 0.0f, 255.0f),	a);				}
		inline constexpr	SColor32		operator /		(float scalar)											const				{ return SColor32((uint8_t)::nwol::clamp(r / scalar, 0.0f, 255.0f)		, (uint8_t)::nwol::clamp(g / scalar, 0.0f, 255.0f)		,(uint8_t)::nwol::clamp(b / scalar, 0.0f, 255.0f),	a);				}
		inline constexpr	SColor32		operator *		(double scalar)											const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r * scalar, 0.0,  255.0 )		, (uint8_t)::nwol::clamp(g * scalar, 0.0 , 255.0 )		,(uint8_t)::nwol::clamp(b * scalar, 0.0, 255.0),	a);				}
		inline constexpr	SColor32		operator /		(double scalar)											const				{ return SColor32((uint8_t)::nwol::clamp(r / scalar, 0.0,  255.0 )		, (uint8_t)::nwol::clamp(g / scalar, 0.0 , 255.0 )		,(uint8_t)::nwol::clamp(b / scalar, 0.0, 255.0),	a);				}
	};	// struct

	typedef					uint16_t		SColor16;

	// Stores BGR color channels
	struct SColor24 {
							uint8_t			b = 0, g = 0, r = 0; 

		inline constexpr					SColor24		()																noexcept	= default;
		inline constexpr					SColor24		(const SColor24& otherColorInt)									noexcept	= default;
		inline constexpr					SColor24		(uint8_t r_, uint8_t g_, uint8_t b_)							noexcept	: b(b_), g(g_), r(r_)																																												{}
		inline constexpr					SColor24		(const SColor32& other)											noexcept	: b(other.b), g(other.b), r(other.r)																																								{}
		inline constexpr					SColor24		(const SColor16& other)											noexcept	: b((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), r((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))					{}
		inline constexpr					SColor24		(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, r((uint8_t)(((other & 0x00FF0000) >> 16)))									{}

		inline constexpr	operator		uint32_t		()														const	noexcept	{ return			0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0);																		}
		inline constexpr	operator		SColor32		()														const	noexcept	{ return SColor32(	0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0));																	}
		inline constexpr	operator		SColor16		()														const	noexcept	{ return (((uint16_t)(b * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(r * (1/255.0f) * 0x001F)) << 11 );											}		
		inline				SColor24&		operator=		(const SColor24& color)											noexcept	= default;
		inline constexpr	bool			operator ==		(const SColor32& other)									const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																				}
		inline constexpr	bool			operator ==		(const SColor24& other)									const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																				}
		inline constexpr	bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColor24(other));																																								}
		inline constexpr	bool			operator !=		(const SColor32& other)									const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																				}
		inline constexpr	bool			operator !=		(const SColor24& other)									const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																				}
		inline constexpr	bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColor24(other));																																								}
		inline constexpr	SColor24		operator *		(float scalar)											const	noexcept	{ return SColor24((uint8_t)::nwol::clamp(r * scalar, 0.0f, 255.0f),	(uint8_t)::nwol::clamp(g * scalar, 0.0f, 255.0f),	(uint8_t)::nwol::clamp(b * scalar, 0.0f, 255.0f));							}
		inline constexpr	SColor24		operator /		(float scalar)											const				{ return SColor24((uint8_t)::nwol::clamp(r / scalar, 0.0f, 255.0f),	(uint8_t)::nwol::clamp(g / scalar, 0.0f, 255.0f),	(uint8_t)::nwol::clamp(b / scalar, 0.0f, 255.0f));							}
		inline constexpr	SColor24		operator *		(double scalar)											const	noexcept	{ return SColor24((uint8_t)::nwol::clamp(r * scalar, 0.0,  255.0), 	(uint8_t)::nwol::clamp(g * scalar, 0.0,  255.0),	(uint8_t)::nwol::clamp(b * scalar, 0.0,  255.0));							}
		inline constexpr	SColor24		operator /		(double scalar)											const				{ return SColor24((uint8_t)::nwol::clamp(r / scalar, 0.0,  255.0), 	(uint8_t)::nwol::clamp(g / scalar, 0.0,  255.0),	(uint8_t)::nwol::clamp(b / scalar, 0.0,  255.0));							}
		inline constexpr	SColor24		operator *		(const SColor24& color)									const	noexcept	{ return SColor24((uint8_t)::nwol::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::nwol::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::nwol::clamp(b * (uint16_t)color.b, 0, 255));				}
		inline constexpr	SColor24		operator +		(const SColor24& color)									const	noexcept	{ return SColor24((uint8_t)::nwol::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::nwol::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::nwol::clamp(b + (uint16_t)color.b, 0, 255));				}
		inline constexpr	SColor32		operator *		(const SColor32& color)									const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::nwol::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::nwol::clamp(b * (uint16_t)color.b, 0, 255), color.a);	}
		inline constexpr	SColor32		operator +		(const SColor32& color)									const	noexcept	{ return SColor32((uint8_t)::nwol::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::nwol::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::nwol::clamp(b + (uint16_t)color.b, 0, 255), color.a);	}
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat {	
							float32_t		r = 0, g = 0, b = 0, a = 0;		// store the color values as floating point ranged in the values (0,1)

		inline constexpr					SColorFloat		()																noexcept	= default;
		inline constexpr					SColorFloat		(const SColorFloat& color)										noexcept	= default;
		inline constexpr					SColorFloat		(float _r, float _g, float _b, float _a=1.0f)					noexcept	: r(_r) ,g(_g) ,b(_b) ,a(_a)																																										{}
		inline constexpr					SColorFloat		(uint16_t Color)												noexcept	: SColorFloat(SColor24(Color))																																										{}
		inline constexpr					SColorFloat		(uint32_t Color)												noexcept	: r(((Color & 0x00FF0000L)>>16)	*(1/255.0f)), g(((Color & 0x0000FF00L)>>8)*(1/255.0f)), b(((Color & 0x000000FFL)>>0)*(1/255.0f)), a(((Color & 0xFF000000L)>>24)*(1/255.0f))							{}
		inline constexpr					SColorFloat		(SColor32 Color)												noexcept	: r(Color.r * (1/255.0f)), g(Color.g*(1/255.0f)), b(Color.b*(1/255.0f)), a(Color.a*(1/255.0f))																										{}
		inline constexpr					SColorFloat		(SColor24 Color)												noexcept	: r(Color.r	* (1/255.0f)), g(Color.g*(1/255.0f)), b(Color.b*(1/255.0f)), a(1.0f)																													{}
		inline constexpr					SColorFloat		(const float* rgbaColor)													: r(rgbaColor[0]), g(rgbaColor[1]), b(rgbaColor[2]), a(rgbaColor[3])																																{}		

		inline				SColorFloat&	operator=		(const SColorFloat& color)													= default;
		inline constexpr	operator		uint32_t		()														const	noexcept	{ return (((uint32_t)(a*255.0f)) << 24) | (((uint32_t)(r*255.0f)) << 16) | (((uint32_t)(g*255.0f)) << 8) | ((uint32_t)(b*255.0f));																	}
		inline constexpr	operator		SColor32		()														const	noexcept	{ return SColor32((uint8_t)(r*255), (uint8_t)(g*255), (uint8_t)(b*255), (uint8_t)(a*255));																											}
		inline constexpr	operator		SColor24		()														const	noexcept	{ return SColor24((uint8_t)(r*255), (uint8_t)(g*255), (uint8_t)(b*255));																															}
		inline constexpr	operator		SColor16		()														const	noexcept	{ return ((((uint16_t)(r*0x1F)) << 11) | (((uint16_t)(g*0x3F)) << 5) | (((uint16_t)(b*0x1F)) << 0));																								}
		inline constexpr	operator		const float*	()														const	noexcept	{ return &r;																																														}
		inline constexpr	bool			operator ==		(const SColorFloat& color)								const	noexcept	{ return ((SColor24)color) == ((SColor24)*this);																																					}
		inline constexpr	bool			operator !=		(const SColorFloat& color)								const	noexcept	{ return ((SColor24)color) != ((SColor24)*this);																																					}
		inline				SColorFloat&	operator +=		(const SColorFloat& color)										noexcept	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return Clamp();																					}
		inline				SColorFloat&	operator *=		(const SColorFloat& color)										noexcept	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return Clamp();																					}
		inline				SColorFloat&	operator *=		(double scalar)													noexcept	{ r = (float)(r * scalar);			g = (float)(g * scalar);		b = (float)(b * scalar);		return Clamp();																					}
		inline				SColorFloat&	operator /=		(double scalar)																{ r = (float)(r / scalar);			g = (float)(g / scalar);		b = (float)(b / scalar);		return Clamp();																					}
		inline				SColorFloat&	operator *=		(const SColor32& color)											noexcept	{ r = r * (color.r*(1/255.0f));		g = g * (color.g*(1/255.0f));	b = b * (color.b*(1/255.0f));	return Clamp();																					}
		inline				SColorFloat&	operator +=		(const SColor32& color)											noexcept	{ r = r + (color.r*(1/255.0f));		g = g + (color.g*(1/255.0f));	b = b + (color.b*(1/255.0f));	return Clamp();																					}
		inline				SColorFloat&	operator *=		(const SColor24& color)											noexcept	{ r = r * (color.r*(1/255.0f));		g = g * (color.g*(1/255.0f));	b = b * (color.b*(1/255.0f));	return Clamp();																					}
		inline				SColorFloat&	operator +=		(const SColor24& color)											noexcept	{ r = r + (color.r*(1/255.0f));		g = g + (color.g*(1/255.0f));	b = b + (color.b*(1/255.0f));	return Clamp();																					}
		inline				SColorFloat		operator *		(const SColor32& color)									const	noexcept	{ return SColorFloat{r * (color.r*(1/255.0f)), g * (color.g*(1/255.0f)), b * (color.b*(1/255.0f)), a}.Clamp();																						}
		inline				SColorFloat		operator +		(const SColor32& color)									const	noexcept	{ return SColorFloat{r + (color.r*(1/255.0f)), g + (color.g*(1/255.0f)), b + (color.b*(1/255.0f)), a}.Clamp();																						}
		inline				SColorFloat		operator *		(const SColor24& color)									const	noexcept	{ return SColorFloat{r * (color.r*(1/255.0f)), g * (color.g*(1/255.0f)), b * (color.b*(1/255.0f)), a}.Clamp();																						}
		inline				SColorFloat		operator +		(const SColor24& color)									const	noexcept	{ return SColorFloat{r + (color.r*(1/255.0f)), g + (color.g*(1/255.0f)), b + (color.b*(1/255.0f)), a}.Clamp();																						}
		inline constexpr	SColorFloat		operator *		(const SColorFloat& color)								const	noexcept	{ return SColorFloat{::nwol::clamp(r * color.r, 0.0f, 1.0f),	::nwol::clamp(g * color.g, 0.0f, 1.0f),	::nwol::clamp(b * color.b, 0.0f, 1.0f)};													}
		inline constexpr	SColorFloat		operator +		(const SColorFloat& color)								const	noexcept	{ return SColorFloat{::nwol::clamp(r + color.r, 0.0f, 1.0f),	::nwol::clamp(g + color.g, 0.0f, 1.0f),	::nwol::clamp(b + color.b, 0.0f, 1.0f)};													}
		inline constexpr	SColorFloat		operator *		(double scalar)											const	noexcept	{ return SColorFloat{(float)::nwol::clamp(r * scalar, 0.0, 1.0), (float)::nwol::clamp(g * scalar, 0.0, 1.0), (float)::nwol::clamp(b * scalar, 0.0, 1.0)};											}
		inline constexpr	SColorFloat		operator /		(double scalar)											const				{ return SColorFloat{(float)::nwol::clamp(r / scalar, 0.0, 1.0), (float)::nwol::clamp(g / scalar, 0.0, 1.0), (float)::nwol::clamp(b / scalar, 0.0, 1.0)};											}
		inline				SColorFloat&	operator *=		(float scalar)													noexcept	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return Clamp();																					}
		inline				SColorFloat&	operator /=		(float scalar)																{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return Clamp();																					}
		inline constexpr	SColorFloat		operator *		(float scalar)											const	noexcept	{ return ::nwol::SColorFloat(::nwol::clamp(r * scalar, 0.0f, 1.0f),	::nwol::clamp(g * scalar, 0.0f, 1.0f),	::nwol::clamp(b * scalar, 0.0f, 1.0f));													}
		inline constexpr	SColorFloat		operator /		(float scalar)											const				{ return ::nwol::SColorFloat(::nwol::clamp(r / scalar, 0.0f, 1.0f),	::nwol::clamp(g / scalar, 0.0f, 1.0f),	::nwol::clamp(b / scalar, 0.0f, 1.0f));													}

		inline				SColorFloat&	Clamp			()																noexcept	{ r = ::nwol::clamp(r, 0.0f, 1.0f); g = ::nwol::clamp(g, 0.0f, 1.0f); b = ::nwol::clamp(b, 0.0f, 1.0f); return *this;																				}
	};	// struct
#pragma pack( pop )

	static constexpr	const ::nwol::SColorFloat	BLACK			= {0.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	WHITE			= {1.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	RED				= {1.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	GREEN			= {0.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	BLUE			= {0.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	YELLOW			= {1.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	MAGENTA			= {1.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	CYAN			= {0.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	ORANGE			= {1.0f, 0.647f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	BROWN			= 0xFF964B00;	
	static constexpr	const ::nwol::SColorFloat	GRAY			= {0.5f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKGRAY		= {0.25f, 0.25f, 0.25f, 1.0f				};
	static constexpr	const ::nwol::SColorFloat	DARKRED			= {0.5f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKGREEN		= {0.0f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKBLUE		= {0.0f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKYELLOW		= {0.5f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKMAGENTA		= {0.5f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKCYAN		= {0.0f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	DARKORANGE		= {1.0f, 0.5490196078431373f, 0.0f, 1.0f	};
	static constexpr	const ::nwol::SColorFloat	LIGHTGRAY		= {0.75f, 0.75f, 0.75f, 1.0f				};
	static constexpr	const ::nwol::SColorFloat	LIGHTRED		= {1.0f, 0.25f, 0.25f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTGREEN		= {0.25f, 1.0f, 0.25f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTBLUE		= {0.25f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTYELLOW		= {1.0f, 1.0f, 0.25f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTMAGENTA	= {1.0f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTCYAN		= {0.25f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::nwol::SColorFloat	LIGHTORANGE		= {1.0f, 0.780f, 0.25f, 1.0f				};

} // namespace


#endif // __COLOR_H__29734982734__
