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

    //Specify a file mask. *.* = We want everything!
    sprintf_s(sPath, "%s\\*.*", ::nwol::glabel(directory, ~0U).c_str());

   if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)	{
        error_printf("Path not found: [%s]", directory);
        return -1;
    }

	::nwol::SPathContents								folder;
    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if (strcmp(fdFile.cFileName,  ".")	!= 0
         && strcmp(fdFile.cFileName, "..")	!= 0)
        {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sprintf_s(sPath, "%s\\%s", directory, fdFile.cFileName);

            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                debug_printf("Directory: %s.", sPath);
				//folder.Folders.push_back(sPath);
            }
            else{ fileNames.push_back(sPath);
                debug_printf("File: %s.", sPath);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

	FindClose(hFind); //Always, Always, clean things up!
#endif

    return 0;
}
