
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

wchar_t* MB2WC(char const* _sIn, DWORD _CodePage = CP_ACP);
bool MB2WC(char const* _sIn, wchar_t** _sOut, DWORD _CodePage = CP_ACP);
bool MB2WC(char const* _sIn, wchar_t* _sOut, size_t& _OutLen, DWORD _CodePage = CP_ACP);
char* WC2MB(wchar_t const* _sIn, DWORD _CodePage = CP_ACP, int _Len = -1);
bool WC2MB(wchar_t const* _sIn, char** _sOut, DWORD _CodePage);
bool WC2MB(wchar_t const* _sIn, char* _sOut, size_t& _OutLen, DWORD _CodePage);
int CompareWideCharString(const wchar_t* _s1, const wchar_t* _s2, DWORD _Len);
size_t MakeString(wchar_t const* _Str, wchar_t** _pVar);

_Success_(return==true) bool IsFileExists(_In_z_ _Const_ const wchar_t* _File);
_Success_(return==true) bool IsFileExists(_In_z_ _Const_ const char* _File);
