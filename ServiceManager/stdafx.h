//=====================================================================================================================================================================
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

//�� Windows ͷ���ų�����ʹ�õ�����
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

//����ϵͳ�汾
#include "targetver.h"

//ĳЩ CString ���캯��������ʽ��
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//�ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

//MFC ��������ͱ�׼���
#include <afxwin.h>

//MFC ��չ
#include <afxext.h>

//MFC �Զ�����
#include <afxdisp.h>

//MFC �� Internet Explorer 4 �����ؼ���֧��
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif

//MFC �� Windows �����ؼ���֧��
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

//�������Ϳؼ����� MFC ֧��
#include <afxcontrolbars.h>

//=====================================================================================================================================================================
//XML֧��
#import "msxml3.dll"
using namespace MSXML2;

#include <comdef.h>
#include <winsvc.h>

//=====================================================================================================================================================================
//Link����
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//=====================================================================================================================================================================
