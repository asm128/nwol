#include "gstring.h"
#include "nwol_math.h"

namespace nwol
{
	::nwol::error_t								parseAbritraryBaseInteger				(uint32_t base, const char* sourceChars, int32_t iChar, int32_t maxChars, int* charsProcessed_, uint64_t* number_, ::nwol::gacstring& stepResult_);
}