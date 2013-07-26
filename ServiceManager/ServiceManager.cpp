//=====================================================================================================================================================================
#include "stdafx.h"
#include "ServiceManager.h"
#include "ServiceManagerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//=====================================================================================================================================================================
//��Ϣӳ��
BEGIN_MESSAGE_MAP(CServiceManagerApp, CWinAppEx)
END_MESSAGE_MAP()

//=====================================================================================================================================================================
//CServiceManagerApp���캯��
CServiceManagerApp::CServiceManagerApp()
{
}

//=====================================================================================================================================================================
//CServiceManagerApp ��ʼ��
BOOL CServiceManagerApp::InitInstance()
{
	//ֻ���е�һʵ��
	HANDLE hMutex=CreateMutex(NULL,FALSE,TEXT("ServicesManagerV1.0byHansen"));
	DWORD dw=WaitForSingleObject(hMutex,0);
	if(dw==WAIT_TIMEOUT)
	{
		//��ʾ��һʵ��
		HWND myHwnd=FindWindow(NULL,TEXT("ServiceManager"));
		if(myHwnd)
		{
			if(IsIconic(myHwnd))
			{
				ShowWindow(myHwnd,SW_RESTORE);
			}

			ShowWindow(myHwnd,SW_SHOW);
			SetForegroundWindow(myHwnd);

			return FALSE;
		}
	}

	//�����ʼ��
	CWinAppEx::InitInstance();

	//ʹ�ÿؼ�����
	AfxEnableControlContainer();

	//��������Ϣ������ע�����
	SetRegistryKey(TEXT("ServiceManager"));

	//=============================================================================================================================================================
	//�ж��Ƿ��в���
	CString strCmdLine = m_lpCmdLine;
	bool bHide = false;

	//û�в�������
	if(strCmdLine.GetLength()>0)
	{
		//ȫ����д
		strCmdLine.MakeUpper();
		//ɾ������
		strCmdLine.Replace(TEXT("\""),TEXT(""));

		if(strCmdLine.Find(TEXT("TRAYICON"))>=0)
		{
			bHide = true;
		}
	}

	//�����Ի��򣬲�������Ϊ������
	CServiceManagerDlg dlg;
	if(bHide)dlg.m_bInitHide=TRUE;
	m_pMainWnd=&dlg;
	return dlg.DoModal();
}

//=====================================================================================================================================================================
//Ψһ��CServiceManagerApp����
CServiceManagerApp theApp;

//=====================================================================================================================================================================
