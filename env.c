#include <windows.h>

FILETIME GetFiles()
{
    FILETIME lastWriteTime = {0};
    WIN32_FIND_DATA FindData;

    HANDLE FindHandle = INVALID_HANDLE_VALUE;
    FindHandle = FindFirstFile("*.c", &FindData);

    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime.dwLowDateTime += FindData.ftLastWriteTime.dwLowDateTime;
        lastWriteTime.dwHighDateTime += FindData.ftLastWriteTime.dwHighDateTime;
    }

    while (FindNextFile(FindHandle, &FindData) != 0)
    {
        lastWriteTime.dwLowDateTime += FindData.ftLastWriteTime.dwLowDateTime;
        lastWriteTime.dwHighDateTime += FindData.ftLastWriteTime.dwHighDateTime;
    }

    FindClose(FindHandle);

    return (lastWriteTime);
}

int main () {
    system("env.bat");

    FILETIME lastFileTime = {0};
    
    while(1) {
        FILETIME newFileTime = GetFiles();
        if (CompareFileTime(&newFileTime, &lastFileTime) != 0)
        {
            system("update.bat");
        }
        lastFileTime = newFileTime;
    }
}