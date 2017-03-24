#ifndef __ASCII_REFERENCE_H__923640__
#define __ASCII_REFERENCE_H__923640__

namespace klib
{
//#pragma warning(disable:4566)
	static const char ascii_wall_double[] = 
		{	' '				// empty				0
		,	char(0xFF & 0xBA)	// vertical				1	║
		,	char(0xFF & 0xB9)	// vertical left		2	╣
		,	char(0xFF & 0xBB)	// down left			3	╗
		,	char(0xFF & 0xC8)	// up right				4	╚
		,	char(0xFF & 0xCA)	// horizontal up		5	╩
		,	char(0xFF & 0xCB)	// horizontal down		6	╦
		,	char(0xFF & 0xCC)	// vertical right		7	╠
		,	char(0xFF & 0xCD)	// horizontal			8	═
		,	char(0xFF & 0xCE)	// cross				9	╬
		,	char(0xFF & 0xBC)	// up left				10	╝
		,	char(0xFF & 0xC9)	// down right			11	╔
		}; //" ╝╗╚╔║═╠╣╦╩╬";	

	static const char ascii_wall_single[] = 
		{	' '				// empty				0	
		,	char(0xFF & 0xB3)	// vertical				1	│
		,	char(0xFF & 0xB4)	// vertical left		2	┤
		,	char(0xFF & 0xBF)	// down left			3	┐
		,	char(0xFF & 0xC0)	// up right				4	└
		,	char(0xFF & 0xC1)	// horizontal up		5	┴
		,	char(0xFF & 0xC2)	// horizontal down		6	┬
		,	char(0xFF & 0xC3)	// vertical right		7	├
		,	char(0xFF & 0xC4)	// horizontal			8	─
		,	char(0xFF & 0xC5)	// cross				9	┼
		,	char(0xFF & 0xD9)	// up left				10	┘
		,	char(0xFF & 0xDA)	// down right			11	┌
		}; //" ┘┐└┌│─├┤┬┴┼";	

	static const char ascii_wall_combined_single[] = 
		{	' '				// empty				0
		,	char(0xFF & 0xB4)	// vertical left	╡	1
		,	char(0xFF & 0xBF)	// down left		╕	2
		,	char(0xFF & 0xC0)	// up right			╘	3
		,	char(0xFF & 0xC1)	// horizontal up	╨	4
		,	char(0xFF & 0xC2)	// horizontal down	╥	5
		,	char(0xFF & 0xC3)	// vertical right	╞	6
		,	char(0xFF &  197)	// cross vertical	╪	7
		,	char(0xFF &  197)	// cross horizontal	╫	8
		,	char(0xFF & 0xD9)	// up left			╛	9
		,	char(0xFF & 0xDA)	// down right		╒	10
		}; // "   "										11

	static const char ascii_wall_combined_reinforced[] = 
		{	' '				// empty				0
		,	char(0xFF & 180)	// vertical left	╢	1
		,	char(0xFF & 191)	// down left		╖	2
		,	char(0xFF & 192)	// up right			╙	3
		,	char(0xFF & 193)	// horizontal up	╧	4
		,	char(0xFF & 194)	// horizontal down	╤	5
		,	char(0xFF & 195)	// vertical right	╟	6
		,	char(0xFF & 197)	// cross vertical	╫	7
		,	char(0xFF & 197)	// cross horizontal	╪	8
		,	char(0xFF & 217)	// up left			╜	9
		,	char(0xFF & 218)	// down right		╓	10
		}; 	//											11

	#define WALL_CONNECTION_CROSS				9
	#define WALL_CORNER_UP_RIGHT				4
	#define WALL_CORNER_UP_LEFT					10
	#define WALL_CORNER_DOWN_RIGHT				11
	#define WALL_CORNER_DOWN_LEFT				3
	#define WALL_VERTICAL						1
	#define WALL_HORIZONTAL						8
	#define WALL_CONNECTION_HORIZONTAL_UP		5
	#define WALL_CONNECTION_HORIZONTAL_DOWN		6
	#define WALL_CONNECTION_VERTICAL_RIGHT		7
	#define WALL_CONNECTION_VERTICAL_LEFT		2

	struct SASCIIWallConnection {
		bool								Bottom				;
		bool								Top					;
		bool								Left				;
		bool								Right				;
		bool								ReinforcedBottom	;
		bool								ReinforcedTop		;
		bool								ReinforcedLeft		;
		bool								ReinforcedRight		;
	};

	static char	resolveASCIIConnectionCross				( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CONNECTION_CROSS			]; } else {	result = ascii_wall_single[WALL_CONNECTION_CROSS			]; } return result; }
	static char	resolveASCIIConnectionHorizontalUp		( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CONNECTION_HORIZONTAL_UP	]; } else {	result = ascii_wall_single[WALL_CONNECTION_HORIZONTAL_UP	]; } return result; }
	static char	resolveASCIIConnectionHorizontalDown	( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CONNECTION_HORIZONTAL_DOWN	]; } else {	result = ascii_wall_single[WALL_CONNECTION_HORIZONTAL_DOWN	]; } return result; }
	static char	resolveASCIIConnectionVerticalRight		( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CONNECTION_VERTICAL_RIGHT	]; } else {	result = ascii_wall_single[WALL_CONNECTION_VERTICAL_RIGHT	]; } return result; }
	static char	resolveASCIIConnectionVerticalLeft		( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CONNECTION_VERTICAL_LEFT	]; } else {	result = ascii_wall_single[WALL_CONNECTION_VERTICAL_LEFT	]; } return result; }
	static char	resolveASCIICornerUpRight				( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CORNER_UP_RIGHT			]; } else {	result = ascii_wall_single[WALL_CORNER_UP_RIGHT				]; } return result; }
	static char	resolveASCIICornerUpLeft				( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CORNER_UP_LEFT				]; } else {	result = ascii_wall_single[WALL_CORNER_UP_LEFT				]; } return result; }
	static char	resolveASCIICornerDownRight				( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CORNER_DOWN_RIGHT			]; } else {	result = ascii_wall_single[WALL_CORNER_DOWN_RIGHT			]; } return result; }
	static char	resolveASCIICornerDownLeft				( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_CORNER_DOWN_LEFT			]; } else {	result = ascii_wall_single[WALL_CORNER_DOWN_LEFT			]; } return result; }
	static char	resolveASCIIVertical					( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_VERTICAL					]; } else {	result = ascii_wall_single[WALL_VERTICAL					]; } return result; }
	static char	resolveASCIIHorizontal					( bool bIsReinforced, const SASCIIWallConnection& connection) { char result; if(bIsReinforced) { result = ascii_wall_double[WALL_HORIZONTAL					]; } else {	result = ascii_wall_single[WALL_HORIZONTAL					]; } return result; }
														 
	template<size_t _Width, size_t _Depth>
	static char							getASCIIWall			(const ::nwol::SGrid<STileProp, _Width, _Depth>& propGrid, int32_t x, int32_t z){
		SASCIIWallConnection					connection				= {false};

		char									result					= '-';
		static const ::nwol::glabel				labelWall				= "Wall";

		bool									bIsReinforced			= propGrid[z][x].Modifier > 0;

		if(x-1 >= 0		&& propGrid[z][x-1].Definition != -1 && definitionsStageProp[propGrid[z][x-1].Definition].Name == labelWall) {	connection.Left		= true;	if(propGrid[z][x-1].Modifier > 0) connection.ReinforcedLeft		= true; }
		if(x+1 < _Width	&& propGrid[z][x+1].Definition != -1 && definitionsStageProp[propGrid[z][x+1].Definition].Name == labelWall) {	connection.Right	= true;	if(propGrid[z][x+1].Modifier > 0) connection.ReinforcedRight	= true; }
		if(z-1 >= 0		&& propGrid[z-1][x].Definition != -1 && definitionsStageProp[propGrid[z-1][x].Definition].Name == labelWall) {	connection.Top		= true;	if(propGrid[z-1][x].Modifier > 0) connection.ReinforcedTop		= true; }
		if(z+1 < _Depth	&& propGrid[z+1][x].Definition != -1 && definitionsStageProp[propGrid[z+1][x].Definition].Name == labelWall) {	connection.Bottom	= true;	if(propGrid[z+1][x].Modifier > 0) connection.ReinforcedBottom	= true; }
		
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

	static const char ascii_fraction[] = 
		{	' '
		,	char(0xFF & 0xAC)
		,	char(0xFF & 0xAB)
		};//'¼', '½'};

	static const char ascii_water[] = {' ', (char)0xF7, '~' };//'≈', '~'};
	static const char ascii_tones[] = {' ', (char)0xB0, (char)0xB1, (char)0xB2}; //" ░▒▓";
	static const char ascii_arrow[] = {0, 0x11, 0x1F, 0x10, 0x1E}; //" ◄▲►▼";
	//static const char ascii_arrw2[] = " ←↑→↓↕↔";

	#define ARROW_LEFT		1
	#define ARROW_UP		2
	#define ARROW_RIGHT		3
	#define ARROW_DOWN		4
	#define ARROW_UPDOWN	5
	#define ARROW_LEFTRIGHT	6

	static const char ascii_gender[] = {' ', 0x0C, 0x0B, (char)232};	// " ♀♂Φ";
	#define FEMALE			1
	#define MALE			2
	#define HERMAPHRODITE	3

	//static const char ascii_sound[] = "♫";

	static const char ascii_cards[] = {' ', 0x03, 0x04, 0x05, 0x06};// " ♥♦♣♠";
	#define DECK_HEARTS		1
	#define DECK_DIAMONDS	2
	#define DECK_CLUBS		3
	#define DECK_SPADES		4

	static const char ascii_face[] = {' ', 0x01, 0x02};//" ☺☻";
	#define FACE_BLACK	1
	#define FACE_WHITE	2

	static const char ascii_button[] = {' ', 0x09, 0x0A}; //" ○◙";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	static const char ascii_buttn2[] = {' ', 0x07, 0x08}; //" •◘";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	//static const char ascii_blah0[] = "'☺☻♥♦♣♠•◘○◙♂♀↨♫ ☼ ►◄↕ ‼ ¶ § ▬ ↨↑↓→←∟↔▲▼ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ⌂ ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬ ½ ¼ ¡« » ░▒▓│┤╣║╗┐└┴┬├─┼╚╔╩╠═╬┘┌ █▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■ ";
//#pragma warning(default:4566)



}

#endif // __ASCII_REFERENCE_H__923640__
