/// Copyright 2016-2017 - asm128
#ifndef ASCII_REFERENCE_H_923640654654
#define ASCII_REFERENCE_H_923640654654

namespace klib
{
#pragma pack(push, 1)
	static const unsigned char			ascii_wall_double				[] = 
		{	' '				// empty				0
		,	0xBA			// vertical				1	║
		,	0xB9			// vertical left		2	╣
		,	0xBB			// down left			3	╗
		,	0xC8			// up right				4	╚
		,	0xCA			// horizontal up		5	╩
		,	0xCB			// horizontal down		6	╦
		,	0xCC			// vertical right		7	╠
		,	0xCD			// horizontal			8	═
		,	0xCE			// cross				9	╬
		,	0xBC			// up left				10	╝
		,	0xC9			// down right			11	╔
		}; //" ╝╗╚╔║═╠╣╦╩╬";	

	static const unsigned char			ascii_wall_single				[] = 
		{	' '				// empty				0	
		,	0xB3			// vertical				1	│
		,	0xB4			// vertical left		2	┤
		,	0xBF			// down left			3	┐
		,	0xC0			// up right				4	└
		,	0xC1			// horizontal up		5	┴
		,	0xC2			// horizontal down		6	┬
		,	0xC3			// vertical right		7	├
		,	0xC4			// horizontal			8	─
		,	0xC5			// cross				9	┼
		,	0xD9			// up left				10	┘
		,	0xDA			// down right			11	┌
		}; //" ┘┐└┌│─├┤┬┴┼";	

	static const unsigned char			ascii_wall_combined_single		[] = 
		{	' '				// empty				0
		,	0xB4			// vertical left	╡	1
		,	0xBF			// down left		╕	2
		,	0xC0			// up right			╘	3
		,	0xC1			// horizontal up	╨	4
		,	0xC2			// horizontal down	╥	5
		,	0xC3			// vertical right	╞	6
		,	 197			// cross vertical	╪	7
		,	 197			// cross horizontal	╫	8
		,	0xD9			// up left			╛	9
		,	0xDA			// down right		╒	10
		}; // "   "										11

	static const unsigned char			ascii_wall_combined_reinforced	[] = 
		{	' '				// empty				0
		,	180				// vertical left	╢	1
		,	191				// down left		╖	2
		,	192				// up right			╙	3
		,	193				// horizontal up	╧	4
		,	194				// horizontal down	╤	5
		,	195				// vertical right	╟	6
		,	197				// cross vertical	╫	7
		,	197				// cross horizontal	╪	8
		,	217				// up left			╜	9
		,	218				// down right		╓	10
		}; 	//											11

	static const unsigned char			ascii_wall_solid				[] = 
		{	' '				// empty				0
		,	180				// vertical left	╢	1
		,	191				// down left		╖	2
		,	192				// up right			╙	3
		,	193				// horizontal up	╧	4
		,	194				// horizontal down	╤	5
		,	195				// vertical right	╟	6
		,	197				// cross vertical	╫	7
		,	197				// cross horizontal	╪	8
		,	217				// up left			╜	9
		,	218				// down right		╓	10
		}; 	//											11

	enum ASCII_WALL : unsigned char
		{ ASCII_WALL_NONE							= 0
		, ASCII_WALL_VERTICAL							// 1
		, ASCII_WALL_CONNECTION_VERTICAL_LEFT			// 2
		, ASCII_WALL_CORNER_DOWN_LEFT					// 3
		, ASCII_WALL_CORNER_UP_RIGHT					// 4
		, ASCII_WALL_CONNECTION_HORIZONTAL_UP			// 5
		, ASCII_WALL_CONNECTION_HORIZONTAL_DOWN			// 6
		, ASCII_WALL_CONNECTION_VERTICAL_RIGHT			// 7
		, ASCII_WALL_HORIZONTAL							// 8
		, ASCII_WALL_CONNECTION_CROSS					// 9
		, ASCII_WALL_CORNER_UP_LEFT						// 10
		, ASCII_WALL_CORNER_DOWN_RIGHT					// 11
		, ASCII_WALL_COUNT								// 12
		, ASCII_WALL_INVALID							= 0xFF// 12
		};										

	struct SASCIIWallConnection {
		bool								Bottom				: 1;
		bool								Top					: 1;
		bool								Left				: 1;
		bool								Right				: 1;
		bool								ReinforcedBottom	: 1;
		bool								ReinforcedTop		: 1;
		bool								ReinforcedLeft		: 1;
		bool								ReinforcedRight		: 1;
	};

	static char	resolveASCIIConnectionCross				( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_CROSS				] : ascii_wall_single[ASCII_WALL_CONNECTION_CROSS				]; }
	static char	resolveASCIIConnectionHorizontalUp		( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_HORIZONTAL_UP		] : ascii_wall_single[ASCII_WALL_CONNECTION_HORIZONTAL_UP		]; }
	static char	resolveASCIIConnectionHorizontalDown	( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_HORIZONTAL_DOWN	] : ascii_wall_single[ASCII_WALL_CONNECTION_HORIZONTAL_DOWN		]; }
	static char	resolveASCIIConnectionVerticalRight		( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_VERTICAL_RIGHT		] : ascii_wall_single[ASCII_WALL_CONNECTION_VERTICAL_RIGHT		]; }
	static char	resolveASCIIConnectionVerticalLeft		( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_VERTICAL_LEFT		] : ascii_wall_single[ASCII_WALL_CONNECTION_VERTICAL_LEFT		]; }
	static char	resolveASCIICornerUpRight				( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_UP_RIGHT				] : ascii_wall_single[ASCII_WALL_CORNER_UP_RIGHT				]; }
	static char	resolveASCIICornerUpLeft				( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_UP_LEFT				] : ascii_wall_single[ASCII_WALL_CORNER_UP_LEFT					]; }
	static char	resolveASCIICornerDownRight				( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_DOWN_RIGHT				] : ascii_wall_single[ASCII_WALL_CORNER_DOWN_RIGHT				]; }
	static char	resolveASCIICornerDownLeft				( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_DOWN_LEFT				] : ascii_wall_single[ASCII_WALL_CORNER_DOWN_LEFT				]; }
	static char	resolveASCIIVertical					( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_VERTICAL						] : ascii_wall_single[ASCII_WALL_VERTICAL						]; }
	static char	resolveASCIIHorizontal					( bool bIsReinforced, const SASCIIWallConnection& connection) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_HORIZONTAL					] : ascii_wall_single[ASCII_WALL_HORIZONTAL						]; }
														 
	template<size_t _sizeWidth, size_t _sizeDepth>
	static char							getASCIIWall			(const ::nwol::SGrid<STileProp, _sizeWidth, _sizeDepth>& propGrid, int32_t x, int32_t z){
		SASCIIWallConnection					connection				= {false};

		char									result					= '-';
		static const ::nwol::glabel				labelWall				= "Wall";

		bool									bIsReinforced			= propGrid[z][x].Modifier > 0;

		if(x-1 >= 0			&& propGrid[z][x-1].Definition != -1 && definitionsStageProp[propGrid[z][x-1].Definition].Name == labelWall) {	connection.Left		= true;	if(propGrid[z][x-1].Modifier > 0) connection.ReinforcedLeft		= true; }
		if(x+1 < _sizeWidth	&& propGrid[z][x+1].Definition != -1 && definitionsStageProp[propGrid[z][x+1].Definition].Name == labelWall) {	connection.Right	= true;	if(propGrid[z][x+1].Modifier > 0) connection.ReinforcedRight	= true; }
		if(z-1 >= 0			&& propGrid[z-1][x].Definition != -1 && definitionsStageProp[propGrid[z-1][x].Definition].Name == labelWall) {	connection.Top		= true;	if(propGrid[z-1][x].Modifier > 0) connection.ReinforcedTop		= true; }
		if(z+1 < _sizeDepth	&& propGrid[z+1][x].Definition != -1 && definitionsStageProp[propGrid[z+1][x].Definition].Name == labelWall) {	connection.Bottom	= true;	if(propGrid[z+1][x].Modifier > 0) connection.ReinforcedBottom	= true; }
		
			 if(connection.Bottom	&& connection.Top		&& connection.Left && connection.Right	) { result = resolveASCIIConnectionCross			(bIsReinforced, connection); }
		else if(connection.Left		&& connection.Right		&& connection.Top						) { result = resolveASCIIConnectionHorizontalUp		(bIsReinforced, connection); }
		else if(connection.Left		&& connection.Right		&& connection.Bottom					) { result = resolveASCIIConnectionHorizontalDown	(bIsReinforced, connection); }
		else if(connection.Top		&& connection.Bottom	&& connection.Right						) { result = resolveASCIIConnectionVerticalRight	(bIsReinforced, connection); }
		else if(connection.Top		&& connection.Bottom	&& connection.Left						) { result = resolveASCIIConnectionVerticalLeft		(bIsReinforced, connection); }
		else if(connection.Top		&& connection.Right												) { result = resolveASCIICornerUpRight				(bIsReinforced, connection); }
		else if(connection.Top		&& connection.Left												) { result = resolveASCIICornerUpLeft				(bIsReinforced, connection); }
		else if(connection.Bottom	&& connection.Right												) { result = resolveASCIICornerDownRight			(bIsReinforced, connection); }
		else if(connection.Bottom	&& connection.Left												) { result = resolveASCIICornerDownLeft				(bIsReinforced, connection); }
		//else iconnection.(bTop	&& connection.Bottom											) { result = resolveASCIIVertical					(bIsReinforced, connection); }
		//else iconnection.(bLeft	&& connection.Right												) { result = resolveASCIIHorizontal					(bIsReinforced, connection); }
		else if(connection.Top		|| connection.Bottom											) { result = resolveASCIIVertical					(bIsReinforced, connection); }
		else if(connection.Left		|| connection.Right												) { result = resolveASCIIHorizontal					(bIsReinforced, connection); }

		return result;
	}

	static const char					ascii_fraction	[]		= {' ',	char(0xFF & 0xAC), char(0xFF & 0xAB) };	//'¼', '½'};
	static const char					ascii_water		[]		= {' ', (char)0xF7, '~' };						//'≈', '~'};
	static const char					ascii_tones		[]		= {' ', (char)0xB0, (char)0xB1, (char)0xB2};	//" ░▒▓";
	static const char					ascii_arrow		[]		= {0, 0x11, 0x1F, 0x10, 0x1E};					//" ◄▲►▼";
	//static const char ascii_arrw2[] = " ←↑→↓↕↔↨";

	#define ARROW_LEFT		1
	#define ARROW_UP		2
	#define ARROW_RIGHT		3
	#define ARROW_DOWN		4
	#define ARROW_UPDOWN	5
	#define ARROW_LEFTRIGHT	6

	static const char					ascii_gender	[]		= {' ', 0x0C, 0x0B, (char)232};				// " ♀♂Φ";
	#define FEMALE			1
	#define MALE			2
	#define HERMAPHRODITE	3

	//static const char ascii_sound[] = "♫";

	static const char					ascii_cards		[]		= {' ', 0x03, 0x04, 0x05, 0x06};				// " ♥♦♣♠";
	#define DECK_HEARTS		1
	#define DECK_DIAMONDS	2
	#define DECK_CLUBS		3
	#define DECK_SPADES		4

	static const char					ascii_face		[]		= {' ', 0x01, 0x02};								//" ☺☻";
	#define FACE_BLACK	1
	#define FACE_WHITE	2

	static const char					ascii_button	[]		= {' ', 0x09, 0x0A};							//" ○◙";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	static const char					ascii_buttn2	[]		= {' ', 0x07, 0x08};							//" •◘";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	// ↑ ↓ → ← 
	// ↔ ↕ ↨ 
	// ⌂
	// ☼
	// « »
	// ª º °
	// ∙
	// ÷
	// ≡
	// ≥ ≤ ±
	// ⌠ ⌡
	// ∞
	// ö Ö
	// ü Ü
	// £ ¥ ₧ $ ¢
	// { | }
	// ƒ 
	// α ß Σ σ µ τ Φ Θ δ Ω φ ε ∩ π 
	// ‼ ¶ § 
	// √ ⁿ ² 
	// ç Ç
	// c C
	// [X] [√]
	// ^ < >
	//static const char ascii_blah0 = "'☺☻♥♦♣♠ •◘ ○◙  ♂ ♀ ↨ ♫ ☼ ►◄ ↕  ▬ ↨∟ ↔ ▲ ▼ !\"#%&'()*+,-./0123456789:;<=>?@ABDEFGHIJKLMNOPQRSTUVWXYZ[\\]_`abdefghijklmnopqrstuvwxyz~ ⌂ éâäàåêëèïîìÄÅÉæÆôòûùÿ áíóúñÑ ¿⌐¬ ½ ¼ ¡ « » ░▒▓│┤╣║╗┐└┴┬├─┼╚╔╩╠═╬┘┌ Γ ÷≈°∙· ";
#pragma pack(pop)
}

#endif // ASCII_REFERENCE_H_923640654654


// ▌ █▐ ▄▌▄▌▐▀■█▄▌▐▀■█▄▌▐▀■█▄▌▐▀■▀■▄▌▐
//    █▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█
//    ▌                            █
//    █                            █
//    █                            █
//    █                            ▐
//    █                            █
//    █                            █
//    █                            █
//    █                            █
//    █                            █
//    █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█

