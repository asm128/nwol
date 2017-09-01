#include "parse_number.h"

// supports up to base 36 by using the abecedary letters as symbols
::nwol::error_t													nwol::parseAbritraryBaseInteger				(uint32_t base, const char* sourceChars, int32_t iChar, int32_t maxChars, int* charsProcessed_, uint64_t* number_, ::nwol::gacstring& stepResult_)			{
	uint32_t															totalCharsProcessed							= 0;
	uint64_t															number										= 0;
	static const char													symbolList []								= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	::nwol::gacstring													stepResult									= {};
	while(iChar < maxChars && sourceChars[iChar] != 0) {
		bool																bSymbolProcessed							= false;
		for( uint32_t iSymbol = 0; iSymbol < base; ++iSymbol ) 
			if( symbolList[iSymbol] == sourceChars[iChar] ) {
				uint64_t															symbolValue									= sourceChars[iChar];
				if( iSymbol > 9 )
					symbolValue														-= 'A' - 10;
				else
					symbolValue														-= '0';

				number															+= symbolValue * ::nwol::powui(base, totalCharsProcessed); 
				bSymbolProcessed												= true;
				nwol_necall(stepResult.append(sourceChars[iChar++]), "Failed to append character! Out of memory?"); 
				++totalCharsProcessed;
				break;
			}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	nwol_necall(stepResult.reverse(), "Cannot reverse parse result string!");	// we assigned the digits backwards so we need to reverse the string.
	nwol_necall(stepResult_.append(stepResult), "Failed to append string! Out of memory?");

	*number_														= number;
	*charsProcessed_												= totalCharsProcessed;
	return 0;
}
