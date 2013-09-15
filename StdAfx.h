// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2CE8F6BA_8B3F_42C3_97C9_4D162C670699__INCLUDED_)
#define AFX_STDAFX_H__2CE8F6BA_8B3F_42C3_97C9_4D162C670699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <afxtempl.h>
#include <Afxmt.h>

#define _CRTDBG_MAP_ALLOC  // 정의를 해줘야 메모리릭 파일 위치까지 나온다.
#include <stdlib.h>  // malloc 함수 사용
#include <crtdbg.h>





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2CE8F6BA_8B3F_42C3_97C9_4D162C670699__INCLUDED_)
