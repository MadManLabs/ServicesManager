//=====================================================================================================================================================================
#include "stdafx.h"
#include "ServiceManager.h"
#include "ServiceManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_TRAY	(WM_USER+100)
#define ERROR_MSGBOX_TITLE (TEXT("We have a big trouble here"))

//=====================================================================================================================================================================
//��Ϣӳ��
BEGIN_MESSAGE_MAP(CServiceManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BT_START, &CServiceManagerDlg::OnBnClickedBtStart)
	ON_BN_CLICKED(IDC_BT_SHUTDOWN, &CServiceManagerDlg::OnBnClickedBtShutdown)
	ON_BN_CLICKED(IDC_BT_RESTART, &CServiceManagerDlg::OnBnClickedBtRestart)
	ON_BN_CLICKED(IDC_BT_DEL, &CServiceManagerDlg::OnBnClickedBtDel)
	ON_BN_CLICKED(IDC_BT_ADD, &CServiceManagerDlg::OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_SAVE, &CServiceManagerDlg::OnBnClickedBtSave)
	ON_BN_CLICKED(IDC_BT_UPDATE, &CServiceManagerDlg::OnBnClickedBtUpdate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TASK, &CServiceManagerDlg::OnLvnItemchangedListTask)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(ID_TRAY,&CServiceManagerDlg::OnTrayNotify)
	ON_COMMAND(ID_POPMENU_EXIT, &CServiceManagerDlg::OnPopmenuExit)
	ON_COMMAND(ID_POPMENU_SHOW, &CServiceManagerDlg::OnPopmenuShow)
	ON_COMMAND(ID_POPMENU_HIDE, &CServiceManagerDlg::OnPopmenuHide)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_POPMENU_SHOW, &CServiceManagerDlg::OnUpdatePopmenuShow)
	ON_UPDATE_COMMAND_UI(ID_POPMENU_HIDE, &CServiceManagerDlg::OnUpdatePopmenuHide)
	ON_WM_NCPAINT()
	ON_COMMAND(ID_POPMENU_AUTOSTART, &CServiceManagerDlg::OnPopmenuAutostart)
	ON_UPDATE_COMMAND_UI(ID_POPMENU_AUTOSTART, &CServiceManagerDlg::OnUpdatePopmenuAutostart)
END_MESSAGE_MAP()

//=====================================================================================================================================================================
//CServiceManagerDlg���캯��
CServiceManagerDlg::CServiceManagerDlg(CWnd* pParent) : CDialog(CServiceManagerDlg::IDD, pParent)
{
	//����ͼ��
	m_hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_pPopMenu=new CMenu();
	m_pPopMenu->LoadMenu(IDR_POPMENU);
	m_pPopMenu=m_pPopMenu->GetSubMenu(0);
}

CServiceManagerDlg* CServiceManagerDlg::m_pThis=NULL;
BOOL CServiceManagerDlg::m_bIsBussy=FALSE;

//=====================================================================================================================================================================
//��ʼ���ؼ�
void CServiceManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASK, m_TaskList);
}

//=====================================================================================================================================================================
//CServiceManagerDlg�����ʼ��
BOOL CServiceManagerDlg::OnInitDialog()
{
	//�����ʼ��
	CDialog::OnInitDialog();

	//���ô˶Ի����ͼ�ꡣ
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//��ȡ�����ļ�·��
	CString	szPath;   
	GetModuleFileName(NULL,szPath.GetBuffer(MAX_PATH),MAX_PATH);
	szPath.ReleaseBuffer();
	int nPos=szPath.ReverseFind('\\');   
	szPath=szPath.Left(nPos +1);
	m_szXML = szPath+TEXT("TaskList.xml");

	//�����б��
	int nRet = m_TaskList.InsertColumn(0, TEXT("������"));
	nRet = m_TaskList.InsertColumn(1, TEXT("����1"));
	nRet = m_TaskList.InsertColumn(2, TEXT("����2"));
	nRet = m_TaskList.InsertColumn(3, TEXT("����3"));
	nRet = m_TaskList.InsertColumn(4, TEXT("����4"));
	nRet = m_TaskList.InsertColumn(5, TEXT("����5"));
	nRet = m_TaskList.InsertColumn(6, TEXT("����"));
	nRet = m_TaskList.InsertColumn(7, TEXT("ɨ����"));
	nRet = m_TaskList.InsertColumn(8, TEXT("�´�ɨ��"));
	
	CRect rect;
	m_TaskList.GetClientRect(&rect);
	int nColumnWidth = (rect.right - rect.left)/3;
	m_TaskList.SetColumnWidth(0, nColumnWidth);
	m_TaskList.SetColumnWidth(1, nColumnWidth);
	m_TaskList.SetColumnWidth(2, nColumnWidth);
	m_TaskList.SetColumnWidth(3, nColumnWidth);
	m_TaskList.SetColumnWidth(4, nColumnWidth);
	m_TaskList.SetColumnWidth(5, nColumnWidth);
	m_TaskList.SetColumnWidth(6, 40);
	m_TaskList.SetColumnWidth(7, 60);
	m_TaskList.SetColumnWidth(8, 60);

	//ȫ��ѡȡ
	m_TaskList.SetExtendedStyle(m_TaskList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	//�������ͼ��
	AddTrayIcon();

	//��ѯ�Ƿ��Զ�����
	m_bAutoStart=QueryAutoRun();

	//��ʼ��COM
	::CoInitialize(NULL);

	m_pThis=this;
	m_nRefreshServicesStatis=REFRESH_INTERVAL;

	//��������
	LoadConfig();

	//������ʱ��
	SetTimer(WM_USER+1024,1000,(TIMERPROC)TimerMonitor);

	return TRUE;
}

//=====================================================================================================================================================================
//�������
void CServiceManagerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// CDialog::OnClose();

	//���ش���
	ShowWindow(SW_HIDE);
}

//=====================================================================================================================================================================
//��������
void CServiceManagerDlg::OnDestroy()
{
	RemoveTrayIcon();
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

//=====================================================================================================================================================================
//���Ʒǿͻ���
void CServiceManagerDlg::OnNcPaint()
{
	// TODO: Add your message handler code here
	CDialog::OnNcPaint();

	if(m_bInitHide==TRUE)
	{
		ShowWindow(SW_HIDE);
	}
}

//=====================================================================================================================================================================
//��������ť
void CServiceManagerDlg::OnBnClickedBtStart()
{
	// TODO: Add your control notification handler code here
	int nIdex = m_TaskList.GetSelectionMark();
	if(nIdex<0)return;

	CString szValue;

	szValue=m_TaskList.GetItemText(nIdex,1);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,2);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,3);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,4);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,5);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}

//=====================================================================================================================================================================
//ֹͣ����ť
void CServiceManagerDlg::OnBnClickedBtShutdown()
{
	// TODO: Add your control notification handler code here
	int nIdex = m_TaskList.GetSelectionMark();
	if(nIdex<0)return;

	CString szValue;

	szValue=m_TaskList.GetItemText(nIdex,1);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,2);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,3);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,4);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,5);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}

//=====================================================================================================================================================================
//��������ť
void CServiceManagerDlg::OnBnClickedBtRestart()
{
	// TODO: Add your control notification handler code here
	int nIdex = m_TaskList.GetSelectionMark();
	if(nIdex<0)return;

	CString szValue;

	//ֹͣ����
	szValue=m_TaskList.GetItemText(nIdex,1);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,2);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,3);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,4);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,5);
	if(szValue.GetLength()>0)StopServiceByName(szValue);

	//��������
	szValue=m_TaskList.GetItemText(nIdex,1);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,2);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,3);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,4);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	szValue=m_TaskList.GetItemText(nIdex,5);
	if(szValue.GetLength()>0)StartServiceByName(szValue);

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}

//=====================================================================================================================================================================
//������ð�ť
void CServiceManagerDlg::OnBnClickedBtAdd()
{
	// TODO: Add your control notification handler code here
	CString szTaskName;
	CString szService1,szService2,szService3,szService4,szService5;
	GetDlgItemText(IDC_TASK_NAME,szTaskName);
	GetDlgItemText(IDC_SERVICE1,szService1);
	GetDlgItemText(IDC_SERVICE2,szService2);
	GetDlgItemText(IDC_SERVICE3,szService3);
	GetDlgItemText(IDC_SERVICE4,szService4);
	GetDlgItemText(IDC_SERVICE5,szService5);

	if(szTaskName.GetLength()<=0 || szService1.GetLength()+szService2.GetLength()+szService3.GetLength()+szService4.GetLength()+szService5.GetLength()<=0)
	{
		MessageBox(TEXT("������������������һ��������"),TEXT("��ʾ"),MB_OK);
	}
	else
	{
		int nIndex=m_TaskList.InsertItem(0, szTaskName);
		m_TaskList.SetItemText(nIndex,1,szService1);
		m_TaskList.SetItemText(nIndex,2,szService2);
		m_TaskList.SetItemText(nIndex,3,szService3);
		m_TaskList.SetItemText(nIndex,4,szService4);
		m_TaskList.SetItemText(nIndex,5,szService5);
	}

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}

//=====================================================================================================================================================================
//ɾ�����ð�ť
void CServiceManagerDlg::OnBnClickedBtDel()
{
	// TODO: Add your control notification handler code here
	int nIdex = m_TaskList.GetSelectionMark();
	if(nIdex<0)return;

	m_TaskList.DeleteItem(nIdex);

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}

//=====================================================================================================================================================================
//�������ð�ť
void CServiceManagerDlg::OnBnClickedBtUpdate()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_TaskList.GetSelectionMark();
	if(nIndex<0)return;

	CString szTaskName;
	CString szService1,szService2,szService3,szService4,szService5,szInterval;
	GetDlgItemText(IDC_TASK_NAME,szTaskName);
	GetDlgItemText(IDC_SERVICE1,szService1);
	GetDlgItemText(IDC_SERVICE2,szService2);
	GetDlgItemText(IDC_SERVICE3,szService3);
	GetDlgItemText(IDC_SERVICE4,szService4);
	GetDlgItemText(IDC_SERVICE5,szService5);
	GetDlgItemText(IDC_MONITOR_INTERVAL,szInterval);

	if(szTaskName.GetLength()>0 || szService1.GetLength()+szService2.GetLength()+szService3.GetLength()+szService4.GetLength()+szService5.GetLength()>0)
	{
		m_TaskList.SetItemText(nIndex,1,szService1);
		m_TaskList.SetItemText(nIndex,2,szService2);
		m_TaskList.SetItemText(nIndex,3,szService3);
		m_TaskList.SetItemText(nIndex,4,szService4);
		m_TaskList.SetItemText(nIndex,5,szService5);
		if(IsDlgButtonChecked(IDC_CHECK_MONITORON))
		{
			m_TaskList.SetItemText(nIndex,6,TEXT("*"));
		}
		else
		{
			m_TaskList.SetItemText(nIndex,6,TEXT(""));
		}
		m_TaskList.SetItemText(nIndex,7,szInterval);
	}

	m_nRefreshServicesStatis=REFRESH_INTERVAL;
}


//=====================================================================================================================================================================
//�������ð�ť
void CServiceManagerDlg::OnBnClickedBtSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		//XMLʵ��
		MSXML2::IXMLDOMDocumentPtr pXml;
		HRESULT hr = pXml.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		if(!SUCCEEDED(hr))
		{
			MessageBox(TEXT("MSXML2::DOMDocument30 Init Error"),ERROR_MSGBOX_TITLE,MB_OK);
			return;
		}

		MSXML2::IXMLDOMElementPtr xmlRoot;
		MSXML2::IXMLDOMElementPtr xmlNode;

		//�������ڵ�
		pXml->raw_createElement((_bstr_t)(char*)"ServiceManager", &xmlRoot);
		pXml->raw_appendChild(xmlRoot, NULL);

		//д������������Ϣ
		int nItemCount = m_TaskList.GetItemCount();
		for(int i=0;i<nItemCount;i++)
		{
			//���������
			CString szValue;
			pXml->raw_createElement((_bstr_t)(char*)"TaskItem", &xmlNode);
			szValue=m_TaskList.GetItemText(i,0);
			xmlNode->Puttext(szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,1);
			xmlNode->setAttribute("Service1", szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,2);
			xmlNode->setAttribute("Service2", szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,3);
			xmlNode->setAttribute("Service3", szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,4);
			xmlNode->setAttribute("Service4", szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,5);
			xmlNode->setAttribute("Service5", szValue.GetBuffer(szValue.GetLength()));
			szValue=m_TaskList.GetItemText(i,6);
			if(szValue==TEXT("*"))
			{
				xmlNode->setAttribute("MonitorOn", TEXT("1"));
			}
			else
			{
				xmlNode->setAttribute("MonitorOn", TEXT("0"));
			}
			szValue=m_TaskList.GetItemText(i,7);
			xmlNode->setAttribute("MonitorInterval", szValue.GetBuffer(szValue.GetLength()));
			xmlRoot->appendChild(xmlNode);
		}

		//�����ļ�
		pXml->save(m_szXML.GetBuffer(m_szXML.GetLength()));
	}
	catch(_com_error &e)
	{
		MessageBox(e.ErrorMessage(),ERROR_MSGBOX_TITLE,MB_OK);
	}
}

//=====================================================================================================================================================================
//�ı�CListControlѡ��
void CServiceManagerDlg::OnLvnItemchangedListTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//�ı��Ҳ���Ϣ
	int nIndex=pNMLV->iItem;
	if(nIndex>=0)
	{
		CString szValue;
		szValue=m_TaskList.GetItemText(nIndex,0);
		SetDlgItemText(IDC_TASK_NAME,szValue);

		szValue=m_TaskList.GetItemText(nIndex,1);
		SetDlgItemText(IDC_SERVICE1,szValue);

		szValue=m_TaskList.GetItemText(nIndex,2);
		SetDlgItemText(IDC_SERVICE2,szValue);

		szValue=m_TaskList.GetItemText(nIndex,3);
		SetDlgItemText(IDC_SERVICE3,szValue);

		szValue=m_TaskList.GetItemText(nIndex,4);
		SetDlgItemText(IDC_SERVICE4,szValue);

		szValue=m_TaskList.GetItemText(nIndex,5);
		SetDlgItemText(IDC_SERVICE5,szValue);

		szValue=m_TaskList.GetItemText(nIndex,6);
		if(szValue==TEXT("*"))
		{
			CheckDlgButton(IDC_CHECK_MONITORON,1);
		}
		else
		{
			CheckDlgButton(IDC_CHECK_MONITORON,0);
		}

		szValue=m_TaskList.GetItemText(nIndex,7);
		SetDlgItemText(IDC_MONITOR_INTERVAL,szValue);
	}
}

//=====================================================================================================================================================================
//����ͼ����Ϣ
LRESULT CServiceManagerDlg::OnTrayNotify(WPARAM wParam,LPARAM lParam)
{
	UINT uMsg = (UINT)lParam; 
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK:
		//��ʾ����
		if(m_bInitHide==TRUE)
		{
			m_bInitHide=FALSE;
		}
		ShowWindow(SW_SHOW);
		break;
	case WM_RBUTTONDOWN:
		//�Ҽ��˵�
		if(m_pPopMenu->m_hMenu)
		{
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow();
			m_pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN,pt.x,pt.y,this,NULL);
		}
		break;
	}

	return TRUE;
}

//=====================================================================================================================================================================
//���̲˵�����
void CServiceManagerDlg::OnPopmenuAutostart()
{
	// TODO: Add your command handler code here
	if(m_bAutoStart==TRUE)
	{
		RemoveAutoRun();
		m_bAutoStart = FALSE;
	}
	else
	{
		SetAutoRun();
		m_bAutoStart = TRUE;
	}
}

//=====================================================================================================================================================================
//����ͼ���˳�
void CServiceManagerDlg::OnPopmenuExit()
{
	// TODO: Add your command handler code here
	RemoveTrayIcon();
	PostQuitMessage(0);
}

//=====================================================================================================================================================================
//����ͼ����ʾ
void CServiceManagerDlg::OnPopmenuShow()
{
	// TODO: Add your command handler code here
	if(m_bInitHide==TRUE)
	{
		m_bInitHide=FALSE;
	}
	ShowWindow(SW_SHOW);
}

//=====================================================================================================================================================================
//����ͼ������
void CServiceManagerDlg::OnPopmenuHide()
{
	// TODO: Add your command handler code here
	ShowWindow(SW_HIDE);
}

//=====================================================================================================================================================================
//����ͼ��˵�����֪ͨ��Dialog���룩
void CServiceManagerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
	if(pPopupMenu->m_hMenu==NULL)return;

	CCmdUI cmdUI;
	cmdUI.m_pOther=NULL;
	cmdUI.m_pMenu=pPopupMenu;
	cmdUI.m_pSubMenu=NULL;

	//�ж��Ƿ�Ϊ����Menu
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu==pPopupMenu->m_hMenu)
	{
		//Ϊ�����˵�Menu
		cmdUI.m_pParentMenu=pPopupMenu;
	}
	else if((hParentMenu=::GetMenu(m_hWnd))!=NULL)
	{
		//���Ƕ����˵�
		CWnd* pParent = this;
		if (pParent!=NULL && (hParentMenu=::GetMenu(pParent->m_hWnd))!=NULL)
		{
			//��hParentMenu�в��Ҷ�Ӧ�Ӳ˵�
			int nIndexMax=::GetMenuItemCount(hParentMenu);
			for(int nIndex=0; nIndex<nIndexMax; nIndex++)
			{
				if(::GetSubMenu(hParentMenu,nIndex)==pPopupMenu->m_hMenu)
				{
					cmdUI.m_pParentMenu=CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	//���ڲ˵���ÿһ����и���
	cmdUI.m_nIndexMax=pPopupMenu->GetMenuItemCount();
	for(cmdUI.m_nIndex=0; cmdUI.m_nIndex<cmdUI.m_nIndexMax; cmdUI.m_nIndex++)
	{
		cmdUI.m_nID=pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
		if (cmdUI.m_nID==0)continue;

		//���²˵�
		if (cmdUI.m_nID==(UINT)-1)
		{
			//������Ӳ˵�
			cmdUI.m_pSubMenu=pPopupMenu->GetSubMenu(cmdUI.m_nIndex);
			
			//������Ч�˵�ID��Sepeter
			if (cmdUI.m_pSubMenu==NULL || (cmdUI.m_nID=cmdUI.m_pSubMenu->GetMenuItemID(0))==0 || cmdUI.m_nID==(UINT)-1)
			{
				continue;
			}

			//���²˵���
			cmdUI.DoUpdate(this, TRUE);
		}
		else
		{
			//û���Ӳ˵�
			cmdUI.m_pSubMenu=NULL;
			cmdUI.DoUpdate(this, FALSE);
		}

		//���ڿ�����ɾMenuItem����Ҫ����cmdUI��m_nIndex��m_nIndexMax
		UINT nCount=pPopupMenu->GetMenuItemCount();
		if (nCount<cmdUI.m_nIndexMax)
		{
			cmdUI.m_nIndex-=(cmdUI.m_nIndexMax-nCount);
			while (cmdUI.m_nIndex<nCount &&  pPopupMenu->GetMenuItemID(cmdUI.m_nIndex)==cmdUI.m_nID)
			{
				cmdUI.m_nIndex++;
			}
		}
		cmdUI.m_nIndexMax = nCount;
	}
}

//=====================================================================================================================================================================
//��������ͼ��˵�
void CServiceManagerDlg::OnUpdatePopmenuAutostart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_bAutoStart==TRUE)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

//=====================================================================================================================================================================
//��������ͼ��˵�
void CServiceManagerDlg::OnUpdatePopmenuShow(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(IsWindowVisible()==TRUE)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

//=====================================================================================================================================================================
//��������ͼ��˵�
void CServiceManagerDlg::OnUpdatePopmenuHide(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
		if(IsWindowVisible()==TRUE)
	{
		pCmdUI->SetCheck(0);
	}
	else
	{
		pCmdUI->SetCheck(1);
	}
}

//=====================================================================================================================================================================
//�Զ��庯��
//=====================================================================================================================================================================
//�������ͼ��
void CServiceManagerDlg::AddTrayIcon()
{
	//��������ͼ��
	NOTIFYICONDATA nd;
	nd.cbSize=sizeof(NOTIFYICONDATA);
	nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nd.hWnd=m_hWnd;
	nd.hIcon=m_hIcon;
	nd.uID=IDR_MAINFRAME;
	nd.uCallbackMessage=ID_TRAY;
	lstrcpy(nd.szTip,TEXT("˫����ʾ����"));
	if(Shell_NotifyIcon(NIM_ADD,&nd)==FALSE)
	{
		ShowMeError();
	}

}

//=====================================================================================================================================================================
//ɾ������ͼ��
void CServiceManagerDlg::RemoveTrayIcon()
{
	//��������ͼ��
	NOTIFYICONDATA nd;
	nd.cbSize=sizeof(NOTIFYICONDATA);
	nd.hWnd=m_hWnd;
	nd.uID=IDR_MAINFRAME;
	
	if(Shell_NotifyIcon(NIM_DELETE,&nd)==FALSE)
	{
		//ShowMeError();
	}
}

//=====================================================================================================================================================================
//����Զ�����
BOOL CServiceManagerDlg::SetAutoRun()
{
	HKEY hKey;
	CString	szPath;
	GetModuleFileName(NULL,szPath.GetBuffer(MAX_PATH),MAX_PATH);
	szPath.ReleaseBuffer();
	szPath = TEXT("\"") + szPath +  TEXT("\" TrayIcon");

	BOOL bRet=FALSE;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),&hKey)==ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey,TEXT("ServiceManager"),0,REG_SZ,(CONST BYTE*)szPath.GetBuffer(szPath.GetLength()),szPath.GetLength()*sizeof(TCHAR))==ERROR_SUCCESS)
		{
			bRet=TRUE;
		}
	}
	RegCloseKey(hKey);

	return bRet;
}

//=====================================================================================================================================================================
//ɾ���Զ�����
BOOL CServiceManagerDlg::RemoveAutoRun()
{
	HKEY hKey;
	BOOL bRet=FALSE;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),&hKey)==ERROR_SUCCESS)
	{
		if(RegDeleteValue(hKey,TEXT("ServiceManager"))==ERROR_SUCCESS)
		{
			bRet=TRUE;
		}
	}
	RegCloseKey(hKey);

	return bRet;
}

//=====================================================================================================================================================================
//��ѯ�Զ�����
BOOL CServiceManagerDlg::QueryAutoRun()
{
	HKEY hKey;
	BOOL bRet=FALSE;
	TCHAR *tcValue = NULL;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),&hKey)==ERROR_SUCCESS)
	{
		DWORD dwCount;
		DWORD dwType;
		LONG lResult=RegQueryValueEx(hKey,TEXT("ServiceManager"),NULL,&dwType,(LPBYTE)tcValue,&dwCount);
		
		tcValue = new TCHAR[dwCount];
		if(RegQueryValueEx(hKey,TEXT("ServiceManager"),NULL,&dwType,(LPBYTE)tcValue,&dwCount)==ERROR_SUCCESS)
		{
			bRet=TRUE;
		}
	}
	RegCloseKey(hKey);

	return bRet;
}

//=====================================================================================================================================================================
//��ʾ��������
void CServiceManagerDlg::ShowMeError()
{
	DWORD	nErrorNo;
	LPTSTR	lptBuffer;

	nErrorNo=GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,nErrorNo,LANG_NEUTRAL,(LPTSTR)&lptBuffer,0,NULL);
	MessageBox(lptBuffer,TEXT("Error"),MB_OK);
	LocalFree(lptBuffer);
}

//=====================================================================================================================================================================
//��XML������������Ϣ
BOOL CServiceManagerDlg::LoadConfig()
{
	try
	{
		//����COMʵ��
		MSXML2::IXMLDOMDocumentPtr pXml;
		HRESULT hr = pXml.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		if(!SUCCEEDED(hr))
		{
			MessageBox(TEXT("MSXML2::DOMDocument30 Init Error"),ERROR_MSGBOX_TITLE,MB_OK);
			return FALSE;
		}

		MSXML2::IXMLDOMElementPtr	pXmlRoot;
		MSXML2::IXMLDOMNodeListPtr	pXmlNodeList; 
		MSXML2::IXMLDOMNodePtr		pXmlNode;
		MSXML2::DOMNodeType		nodeType;
		MSXML2::IXMLDOMNamedNodeMapPtr	pAttrs;
		MSXML2::IXMLDOMNodePtr		pAttrItem;

		//����XML
		pXml->load(m_szXML.GetBuffer(m_szXML.GetLength()));
		
		//��ȡTaskList����
		pXmlRoot = (MSXML2::IXMLDOMElementPtr)(pXml->selectSingleNode("ServiceManager"));
		if(pXmlRoot == NULL)return TRUE;

		//��ȡ����TaskList
		pXmlNodeList=pXmlRoot->GetchildNodes();
		long lNodeCount=0l;
		pXmlNodeList->get_length(&lNodeCount);

		//�����ڵ�
		for(long i=0l;i<lNodeCount;i++)
		{
			pXmlNodeList->get_item(i,&pXmlNode);

			long lAttributeCount=0l;
			BSTR bstrNodeName;
			pXmlNode->get_text(&bstrNodeName);
			pXmlNode->get_nodeType(&nodeType);
			
			if(MSXML2::NODE_ELEMENT==nodeType)
			{
				pXmlNode->get_attributes(&pAttrs);
				pAttrs->get_length(&lAttributeCount);
				m_TaskList.InsertItem(i, (LPCTSTR)bstrNodeName);

				//��������
				for(long j = 0; j < lAttributeCount; j++)
				{
					pAttrs->get_item(j, &pAttrItem);
					CString strAttrName = pAttrItem->nodeName;

					if(strAttrName.CompareNoCase(TEXT("Service1"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 1, strAttrValue);
					}
					else if(strAttrName.CompareNoCase(TEXT("Service2"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 2, strAttrValue);
					}
					else if(strAttrName.CompareNoCase(TEXT("Service3"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 3, strAttrValue);
					}
					else if(strAttrName.CompareNoCase(TEXT("Service4"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 4, strAttrValue);
					}
					else if(strAttrName.CompareNoCase(TEXT("Service5"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 5, strAttrValue);
					}
					else if(strAttrName.CompareNoCase(TEXT("MonitorOn"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						if(strAttrValue==TEXT("1"))
						{
							m_TaskList.SetItemText(i, 6, TEXT("*"));
						}
						else
						{
							m_TaskList.SetItemText(i, 6, TEXT(""));
						}
					}
					else if(strAttrName.CompareNoCase(TEXT("MonitorInterval"))==0)
					{
						CString strAttrValue = (_bstr_t)pAttrItem->nodeTypedValue;
						m_TaskList.SetItemText(i, 7, strAttrValue);
					}
				}
			}
		}
	}
	catch(_com_error &e)
	{
		MessageBox(e.ErrorMessage(),ERROR_MSGBOX_TITLE,MB_OK);
		return FALSE;
	}

	return TRUE;
}

//=====================================================================================================================================================================
//���ݷ�������������
BOOL CServiceManagerDlg::StartServiceByName(CString szServiceName)
{
	//SCMD��Service�ľ��
	SC_HANDLE	hSCManager=NULL;
	SC_HANDLE	hService=NULL;
	
	//����״̬
	SERVICE_STATUS_PROCESS	sspServiceStatus; 
	DWORD	dwBytesNeeded;
	
	//�ȴ�ʱ��
	DWORD	dwOldCheckPoint; 
	DWORD	dwStartTickCount;
	DWORD	dwWaitTime;
 
	//�򿪷��������
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCManager) 
	{
		//MessageBox(TEXT("OpenSCManager Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		return FALSE;
	}

	//��ȡ������
	hService = OpenService(hSCManager,szServiceName,SERVICE_ALL_ACCESS);
	if(hService == NULL)
	{ 
		//MessageBox(TEXT("OpenService Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	//��ѯ����״̬
	if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
	{
		//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}

	//��������Ѿ�����
	if(sspServiceStatus.dwCurrentState != SERVICE_STOPPED && sspServiceStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return TRUE; 
	}

	//�����������ֹͣ���ȴ���ֹͣ
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
	if(dwOldCheckPoint!=0)
	{
		while (sspServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			//�ȴ�һ��ʱ��
			dwWaitTime = sspServiceStatus.dwWaitHint/10;
			if(dwWaitTime<1000)
			{
				dwWaitTime = 1000;
			}
			else if (dwWaitTime>10000)
			{
				dwWaitTime = 10000;
			}
			Sleep( dwWaitTime );

			//������ѯ����״̬
			if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
			{
				//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return FALSE; 
			}

			if(sspServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				if(sspServiceStatus.dwCheckPoint>dwOldCheckPoint)
				{
					//�����ȴ�
					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
				}
				else
				{
					//�ȴ���ʱ
					if(GetTickCount()-dwStartTickCount>sspServiceStatus.dwWaitHint)
					{
						//MessageBox(TEXT("Timeout waiting for service to stop"),ERROR_MSGBOX_TITLE,MB_OK);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return FALSE; 
					}
				}
			}
		}
	}

	//��������
	if (!StartService(hService,0,NULL))
	{
		//MessageBox(TEXT("StartService Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}

	//������״̬
	if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
	{
		//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}
 
	//�ȴ������������
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
	if(dwOldCheckPoint!=0)
	{
		while (sspServiceStatus.dwCurrentState == SERVICE_START_PENDING) 
		{
			//�ȴ�һ��ʱ��
 			dwWaitTime = sspServiceStatus.dwWaitHint/10;
			if( dwWaitTime < 1000 )
			{
				dwWaitTime = 1000;
			}
			else if ( dwWaitTime > 10000 )
			{
				dwWaitTime = 10000;
			}
			Sleep( dwWaitTime );

			//������״̬
			if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded ) )			  // if buffer too small
			{
				//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return FALSE; 
			}
	 
			if(sspServiceStatus.dwCurrentState == SERVICE_START_PENDING)
			{
				if ( sspServiceStatus.dwCheckPoint > dwOldCheckPoint )
				{
					//�����ȴ�
					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
				}
				else
				{
					//�ȴ���ʱ
					if(GetTickCount()-dwStartTickCount > sspServiceStatus.dwWaitHint)
					{
						//MessageBox(TEXT("Timeout waiting for service to start up"),ERROR_MSGBOX_TITLE,MB_OK);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return FALSE;
					}
				}
			}
		}
	}

	//�رվ��
	CloseServiceHandle(hService); 
	CloseServiceHandle(hSCManager);
	return TRUE;
}

//=====================================================================================================================================================================
//���ݷ�����ֹͣ����
BOOL CServiceManagerDlg::StopServiceByName(CString szServiceName)
{
	//SCMD��Service�ľ��
	SC_HANDLE	hSCManager=NULL;
	SC_HANDLE	hService=NULL;
	
	//����״̬
	SERVICE_STATUS_PROCESS	sspServiceStatus; 
	DWORD	dwBytesNeeded;
	
	//�ȴ�ʱ��
	DWORD	dwOldCheckPoint; 
	DWORD	dwStartTickCount;
	DWORD	dwWaitTime;
 
	//�򿪷��������
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCManager) 
	{
		//MessageBox(TEXT("OpenSCManager Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		return FALSE;
	}

	//��ȡ������
	hService = OpenService(hSCManager,szServiceName,SERVICE_ALL_ACCESS);
	if(hService == NULL)
	{ 
		//MessageBox(TEXT("OpenService Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	//��ѯ����״̬
	if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
	{
		//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}

	//��������Ѿ�ֹͣ
	if(sspServiceStatus.dwCurrentState != SERVICE_RUNNING && sspServiceStatus.dwCurrentState != SERVICE_START_PENDING)
	{
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return TRUE; 
	}

	//�ȴ������������
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
	if(dwOldCheckPoint!=0)
	{
		while (sspServiceStatus.dwCurrentState == SERVICE_START_PENDING) 
		{
			//�ȴ�һ��ʱ��
 			dwWaitTime = sspServiceStatus.dwWaitHint/10;
			if( dwWaitTime < 1000 )
			{
				dwWaitTime = 1000;
			}
			else if ( dwWaitTime > 10000 )
			{
				dwWaitTime = 10000;
			}
			Sleep( dwWaitTime );

			//������״̬
			if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded ) )			  // if buffer too small
			{
				//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return FALSE; 
			}
	 
			if(sspServiceStatus.dwCurrentState == SERVICE_START_PENDING)
			{
				if ( sspServiceStatus.dwCheckPoint > dwOldCheckPoint )
				{
					//�����ȴ�
					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
				}
				else
				{
					//�ȴ���ʱ
					if(GetTickCount()-dwStartTickCount > sspServiceStatus.dwWaitHint)
					{
						//MessageBox(TEXT("Timeout waiting for service to start up"),ERROR_MSGBOX_TITLE,MB_OK);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return FALSE;
					}
				}
			}
		}
	}

	//ֹͣ����
	SERVICE_STATUS SvcStatus;
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &SvcStatus))
	{
		//MessageBox(TEXT("StartService Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}

	//������״̬
	if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
	{
		//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}
 
	//�����������ֹͣ���ȴ���ֹͣ
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
	if(dwOldCheckPoint!=0)
	{
		while (sspServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			//�ȴ�һ��ʱ��
			dwWaitTime = sspServiceStatus.dwWaitHint/10;
			if(dwWaitTime<1000)
			{
				dwWaitTime = 1000;
			}
			else if (dwWaitTime>10000)
			{
				dwWaitTime = 10000;
			}
			Sleep( dwWaitTime );

			//������ѯ����״̬
			if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
			{
				//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
				CloseServiceHandle(hService); 
				CloseServiceHandle(hSCManager);
				return FALSE; 
			}

			if(sspServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				if(sspServiceStatus.dwCheckPoint>dwOldCheckPoint)
				{
					//�����ȴ�
					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = sspServiceStatus.dwCheckPoint;
				}
				else
				{
					//�ȴ���ʱ
					if(GetTickCount()-dwStartTickCount>sspServiceStatus.dwWaitHint)
					{
						//MessageBox(TEXT("Timeout waiting for service to stop"),ERROR_MSGBOX_TITLE,MB_OK);
						CloseServiceHandle(hService); 
						CloseServiceHandle(hSCManager);
						return FALSE; 
					}
				}
			}
		}
	}

	//�رվ��
	CloseServiceHandle(hService); 
	CloseServiceHandle(hSCManager);
	return TRUE;
}

//=====================================================================================================================================================================
//���ݷ��������жϷ����Ƿ��Ѿ�����
BOOL CServiceManagerDlg::IsServiceOnByName(CString szServiceName)
{
	//SCMD��Service�ľ��
	SC_HANDLE	hSCManager=NULL;
	SC_HANDLE	hService=NULL;
	
	//����״̬
	SERVICE_STATUS_PROCESS	sspServiceStatus; 
	DWORD	dwBytesNeeded;
 
	//�򿪷��������
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCManager) 
	{
		//MessageBox(TEXT("OpenSCManager Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		return FALSE;
	}

	//��ȡ������
	hService = OpenService(hSCManager,szServiceName,SERVICE_ALL_ACCESS);
	if(hService == NULL)
	{ 
		//MessageBox(TEXT("OpenService Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	//��ѯ����״̬
	if (!QueryServiceStatusEx(hService,SC_STATUS_PROCESS_INFO,(LPBYTE) &sspServiceStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
	{
		//MessageBox(TEXT("QueryServiceStatusEx Failed"),ERROR_MSGBOX_TITLE,MB_OK);
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return FALSE; 
	}

	//��������Ѿ�����
	if(sspServiceStatus.dwCurrentState != SERVICE_STOPPED && sspServiceStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		CloseServiceHandle(hService); 
		CloseServiceHandle(hSCManager);
		return TRUE; 
	}

	CloseServiceHandle(hService); 
	CloseServiceHandle(hSCManager);
	return FALSE;
}

//=====================================================================================================================================================================
//��ʱ���ص�����
VOID CALLBACK CServiceManagerDlg::TimerMonitor(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	if(m_bIsBussy==TRUE || m_pThis==NULL)return;
	m_bIsBussy=TRUE;

	//���ӷ�������
	int nItemCount = m_pThis->m_TaskList.GetItemCount();
	CString szMonitorOn;
	CString szMonitorInterval;
	CString szMonitorRefresh;
	CString szValue;
	int nMonitorInterval=0;
	int nMonitorRefresh=0;

	for(int i=0;i<nItemCount;i++)
	{
		szMonitorOn=m_pThis->m_TaskList.GetItemText(i,6);
		szMonitorInterval=m_pThis->m_TaskList.GetItemText(i,7);
		szMonitorRefresh=m_pThis->m_TaskList.GetItemText(i,8);

		nMonitorInterval=_ttoi(szMonitorInterval);
		nMonitorRefresh=_ttoi(szMonitorRefresh);

		if(szMonitorOn==TEXT("*") && nMonitorInterval>0)
		{
			if(nMonitorRefresh==0)
			{
				nMonitorRefresh=nMonitorInterval;
			}
			else if(nMonitorRefresh==1)
			{
				//����������������ȫ������
				szValue=m_pThis->m_TaskList.GetItemText(i,1);
				if(szValue.GetLength()>0)StartServiceByName(szValue);

				szValue=m_pThis->m_TaskList.GetItemText(i,2);
				if(szValue.GetLength()>0)StartServiceByName(szValue);

				szValue=m_pThis->m_TaskList.GetItemText(i,3);
				if(szValue.GetLength()>0)StartServiceByName(szValue);

				szValue=m_pThis->m_TaskList.GetItemText(i,4);
				if(szValue.GetLength()>0)StartServiceByName(szValue);

				szValue=m_pThis->m_TaskList.GetItemText(i,5);
				if(szValue.GetLength()>0)StartServiceByName(szValue);

				nMonitorRefresh=nMonitorInterval;
			}
			else
			{
				nMonitorRefresh--;
			}

			szValue.Format(TEXT("%d"),nMonitorRefresh);
			m_pThis->m_TaskList.SetItemText(i,8,szValue);
		}
	}

	//��ʱ������״̬
	if(m_pThis->m_nRefreshServicesStatis>=REFRESH_INTERVAL)
	{
		for(int i=0;i<nItemCount;i++)
		{
			for(int j=1;j<6;j++)
			{
				CString szServicesName = m_pThis->m_TaskList.GetItemText(i,j);
				if(IsServiceOnByName(szServicesName))
				{
					m_pThis->m_TaskList.setSubItemColor(i,j,CNeoSubItemColor::GreenColor);
				}
				else
				{
					m_pThis->m_TaskList.setSubItemColor(i,j,CNeoSubItemColor::RedColor);
				}
			}
		}

		m_pThis->m_TaskList.RedrawWindow();
		m_pThis->m_nRefreshServicesStatis=0;
	}
	else
	{
		m_pThis->m_nRefreshServicesStatis++;
	}


	m_bIsBussy=FALSE;
}

//=====================================================================================================================================================================
