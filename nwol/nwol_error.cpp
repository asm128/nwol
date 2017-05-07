#include "nwol_error.h"

#if defined (__WINDOWS__)
#include <windows.h>
#endif

::std::string		nwol::getWindowsErrorAsString		(uint64_t lastError)					{
	if(lastError == 0)	
		return ::std::string(); // No error message has been recorded

	// Get the error message, if any.
	LPSTR											messageBuffer									= nullptr;
	const size_t									size											= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	if(nullptr != messageBuffer) {
		const ::std::string								message											(messageBuffer, size >= 2 ? size-2 : size);
		LocalFree(messageBuffer); 
		return message;
	}

	return ::std::string();
}