#define WINVER 0x0601
#define _WIN32_WINNT WINVER
#define NTDDI_VERSION 0x06010000
#define UNICODE
//------------------------------------------------------------------------------

#include <stdio.h>
#include <windows.h>
#include <Shlobj.h>
#include <Knownfolders.h>
//------------------------------------------------------------------------------

int main(){
  wchar_t* Path   = 0;
  HRESULT  Result = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, 0, &Path);

  wprintf(L"Result = %d; Path = %ls\n", Result, Path);

  return 0;
}
//------------------------------------------------------------------------------

