/// Copyright 2016-2017 - asm128
#ifndef ASCII_COLOR_H_1209387102983710923709128370192873
#define ASCII_COLOR_H_1209387102983710923709128370192873

namespace nwol
{
	struct STextColorASCII {
							uint8_t													Foreground			: 4;
							uint8_t													Background			: 4;

		//inline				operator												uint8_t				()	const	noexcept	{ return *(const uint8_t*)this;				}	
		inline	constexpr	operator												uint8_t				()	const	noexcept	{ return Foreground | (Background << 4);	}	
	};
} // namespace

#define	COLOR_0		0x000000	// BLACK				
#define	COLOR_1 	0x800000	// DARKBLUE			
#define	COLOR_2 	0x008000	// DARKGREEN			
#define	COLOR_3 	0x808000	// DARKCYAN			
#define	COLOR_4 	0x000080   	// DARKRED			
#define	COLOR_5 	0x800080	// DARKMAGENTA		
#define	COLOR_6 	0x008080	// DARKYELLOW		
#define	COLOR_7 	0x00A5FF	// 0xC0C0C0	// LIGHTGREY or ORANGE		   
#define	COLOR_8 	0x808080	// DARKGREY			
#define	COLOR_9 	0xFF0000	// BLUE				
#define	COLOR_10	0x00FF00	// GREEN				
#define	COLOR_11	0xFFFF00	// CYAN				
#define	COLOR_12	0x0000FF	// RED				
#define	COLOR_13	0xFF00FF	// MAGENTA			
#define	COLOR_14	0x00FFFF	// YELLOW			
#define	COLOR_15	0xFFFFFF	// WHITE				
								
#define COLOR_BLACK				 0		 
#define COLOR_DARKBLUE			 1 		 
#define COLOR_DARKGREEN			 2 		 
#define COLOR_DARKCYAN			 3 		 
#define COLOR_DARKRED			 4 		 
#define COLOR_DARKMAGENTA		 5 		 
#define COLOR_DARKYELLOW		 6 		 
#define COLOR_LIGHTGREY			 7 		 
#define COLOR_DARKGREY			 8 		 
#define COLOR_BLUE				 9 		 
#define COLOR_GREEN				10		 
#define COLOR_CYAN				11		 
#define COLOR_RED				12		 
#define COLOR_MAGENTA			13		 
#define COLOR_YELLOW			14		 
#define COLOR_WHITE				15		 

#define COLOR_ORANGE COLOR_LIGHTGREY

// BLINK	128 

#define invert_color_ascii(_col) (((_col & 0xF0)>>4) | ((_col & 0xF)<<4))

#endif // ASCII_COLOR_H_1209387102983710923709128370192873
