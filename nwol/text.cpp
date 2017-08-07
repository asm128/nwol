#include "nwol_text.h"

int32_t								getLines				(const char* source, int32_t maxLen, ::nwol::array_obj<std::string>& lines_)				{
		::nwol::array_obj<std::string>			lines;
		int32_t									bufferIndex				= 0;
		while(bufferIndex < maxLen) {
			char									nameBuffer[256]			= {};
			int32_t									nameLen					= 0;
			while(nameLen < 256 && bufferIndex < maxLen) {
				nameBuffer[nameLen++]				= source[bufferIndex++];
				if(nameBuffer[nameLen - 1] == '\n') {
					nameBuffer[nameLen - ((nameLen > 1 && nameBuffer[nameLen - 2] == '\r') ? 2 : 1)]	= 0;
					break;
				}
			}
			lines.push_back(nameBuffer);
		}
		return lines_						= lines;
	}
