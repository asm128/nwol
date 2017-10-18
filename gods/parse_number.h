#include "gstring.h"

namespace gods
{
	::nwol::error_t					parseArbitraryBaseInteger		(uint32_t base, const char* symbolList, const char* sourceChars, int32_t offsetChar, int32_t maxChars, int* charsProcessed_, uint64_t* number_, ::nwol::gacstring& stepResult_);
}