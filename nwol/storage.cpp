#include "storage.h"

#if defined(__WINDOWS__)
#include <Windows.h>
#endif

int32_t										nwol::listFiles								(const char* directory, ::nwol::array_obj<::nwol::glabel>& fileNames)
{
 #if defined(__WINDOWS__)
    WIN32_FIND_DATA										fdFile;
    HANDLE												hFind									= NULL;

    char												sPath[4096];

    sprintf_s(sPath, "%s\\*.*", ::nwol::glabel(directory, ~0U).c_str());	//Specify a file mask. *.* = We want everything!

	hFind											= FindFirstFile(sPath, &fdFile);
	reterr_error_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s]", directory);

	::nwol::SPathContents								folder;
    do { // Find first file will always return "." and ".." as the first two directories.
        if	(	0 != strcmp(fdFile.cFileName,  ".")
			&&	0 != strcmp(fdFile.cFileName, "..")
			)
        {
            sprintf_s(sPath, "%s\\%s", directory, fdFile.cFileName);	// Build up our file path using the passed in [directory] and the file/foldername we just found.

            if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                info_printf("Directory: %s.", sPath);
				//folder.Folders.push_back(sPath);
            }
            else { 
				reterr_error_if(-1 == fileNames.push_back(sPath), "Failed to push path to output list");
                info_printf("File: %s.", sPath);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

	FindClose(hFind); //Always, Always, clean things up!
#endif

    return 0;
}
