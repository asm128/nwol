/// Copyright 2017 - asm128
#include "nwol_log.h"

#if defined (__WINDOWS__)
#	include <windows.h>
#endif

::std::string								nwol::getWindowsErrorAsString		(const uint64_t lastError)					{	// Get the error message, if any.
	if(0 == lastError) 
		return {};
	char											* messageBuffer						= nullptr;
	const size_t									size								= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	retval_error_if({}, nullptr == messageBuffer, "FormatMessage() failed.")
	else {
		const ::std::string								message								(messageBuffer, size >= 2 ? size-2 : size);
		LocalFree(messageBuffer); 
		return message;
	}
}
