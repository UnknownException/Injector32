#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>

// Common controls
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
                                              name                  = 'Microsoft.Windows.Common-Controls' \
                                              version               = '6.0.0.0' \
                                              processorArchitecture = '*' \
                                              publicKeyToken        = '6595b64144ccf1df' \
                                              language              = '*'\"")

#include <shellapi.h>