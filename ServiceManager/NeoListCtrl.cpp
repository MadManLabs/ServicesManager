// NeoListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ServiceManager.h"
#include "NeoListCtrl.h"


// CNeoListCtrl

IMPLEMENT_DYNAMIC(CNeoListCtrl, CListCtrl)

CNeoListCtrl::CNeoListCtrl()
{
}

CNeoListCtrl::~CNeoListCtrl()
{
}

BEGIN_MESSAGE_MAP(CNeoListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CNeoListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()

//����ÿ��SubItem����ɫ
void CNeoListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{	
	NMLVCUSTOMDRAW* pNMLVCCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT ==pNMLVCCD->nmcd.dwDrawStage)
	{
		//֪ͨItem
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMLVCCD->nmcd.dwDrawStage)
	{
		//֪ͨSubItem
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pNMLVCCD->nmcd.dwDrawStage)
	{
		//����SubItem
		int nItem = static_cast<int>(pNMLVCCD->nmcd.dwItemSpec);
		int nSubItem = pNMLVCCD->iSubItem;
		pNMLVCCD->clrText = getSubItemColor(nItem,nSubItem);
	}
}
