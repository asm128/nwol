#include "parse_number.h"
#include "nwol_math.h"

::nwol::error_t													gods::parseArbitraryBaseInteger				(uint32_t base, const char* symbolList, const char* sourceChars, int32_t iChar, int32_t maxChars, int* charsProcessed_, uint64_t* number_, ::nwol::gacstring& stepResult_)	{
	uint32_t															totalCharsProcessed							= 0;
	uint64_t															number										= 0;
	::nwol::gacstring													stringToParse								= {};
	while(iChar < maxChars && sourceChars[iChar] != 0) {
		bool																bSymbolProcessed							= false;
		for(uint32_t iSymbol = 0; iSymbol < base; ++iSymbol)
			if( symbolList[iSymbol] == sourceChars[iChar] ) {
				bSymbolProcessed												= true;
				nwol_necall(stringToParse.append(sourceChars[iChar++]), "Failed to append character! Out of memory?"); 
				++totalCharsProcessed;
				break;
			}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	iChar															-= totalCharsProcessed;
	totalCharsProcessed												= 0;
	nwol_necall(stringToParse.reverse(), "Cannot reverse string for parsing! How could this ever happen?");		// we assigned the digits backwards so we need to reverse the string.
	::nwol::gacstring													stepResult									= {};
	while(iChar < maxChars && stringToParse[iChar] != 0) {
		bool																bSymbolProcessed							= false;
		for( uint32_t iSymbol = 0; iSymbol < base; ++iSymbol ) 
			if( symbolList[iSymbol] == stringToParse[iChar] ) {
				number															+= iSymbol * ::nwol::powui(base, totalCharsProcessed); 
				bSymbolProcessed												= true;
				nwol_necall(stepResult.append(stringToParse[iChar++]), "Failed to append character! Out of memory?"); 
				++totalCharsProcessed;
				break;
			}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	nwol_necall(stepResult.reverse(), "Cannot reverse parse setp result! Out of memory?");		// we assigned the digits backwards so we need to reverse the string.
	nwol_necall(stepResult_.append(stepResult), "Failed to append string! Out of memory?");

	*number_														= number;
	*charsProcessed_												= totalCharsProcessed;
	return 0;
}

