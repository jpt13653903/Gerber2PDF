# Notes

Various useful titbits of information.

## LocalAppData.cpp

Used in Windows to obtain the path to the current user's local app-data folder.  Build with:

`g++ LocalAppData.cpp -luuid -o LocalAppData.exe`

See [here](https://docs.microsoft.com/en-us/windows/desktop/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath) for more information.

