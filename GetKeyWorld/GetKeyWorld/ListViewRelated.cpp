#include "stdafx.h"

MyListView::MyListView(char** strColumnName, HWND hModuleTable)
{
	int columnIndex = 0;
	while (*strColumnName != NULL)
	{
		columnName.Add(*strColumnName, columnIndex);
		strColumnName++;
		columnIndex++;
	}
	this->hModuleTable = hModuleTable;
}

LRESULT MyListView::ListViewAddColumn(int nColumn, int nWidth, const CHAR* lpszHead)
{
	LV_COLUMN stLVC;
	RtlZeroMemory(&stLVC, sizeof(LV_COLUMN));
	stLVC.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	stLVC.fmt = LVCFMT_LEFT;
	stLVC.pszText = (char*)lpszHead;
	stLVC.cx = nWidth;
	stLVC.iSubItem = nColumn;
	return SendMessage(hModuleTable, LVM_INSERTCOLUMN, nColumn, (LPARAM)&stLVC);
}

//在ListView中新增一行，或修改一行中某个字段的内容
//输入：_dwItem = 要修改的行的编号
//_dwSubItem = 要修改的字段的编号， - 1表示插入新的行， >= 1表示字段的编号
LRESULT MyListView::ListViewSetItem(int nItem, int nSubItem, TCHAR* lpszText)
{
	RtlZeroMemory(&stLVI, sizeof(LV_ITEM));
	int length = lstrlen(lpszText);
	stLVI.cchTextMax = length;
	stLVI.mask = LVIF_TEXT;
	stLVI.pszText = lpszText;
	stLVI.iItem = nItem;
	stLVI.iSubItem = nSubItem;

	if (nSubItem == -1)
	{
		stLVI.iSubItem = 0;
		return SendMessage(hModuleTable, LVM_INSERTITEM, NULL, (LPARAM)&stLVI);
	}
	else
	{
		return SendMessage(hModuleTable, LVM_SETITEM, NULL, (LPARAM)&stLVI);
	}
}

//清除ListView中的内容
//删除所有的行和所有的列
void MyListView::ListViewClear()
{
	SendMessage(hModuleTable, LVM_DELETEALLITEMS, 0, 0);
	while (TRUE)
	{
		LRESULT lrResult = SendMessage(hModuleTable, LVM_DELETECOLUMN, 0, 0);
		if (!lrResult)
		{
			break;
		}
	}
}

//返回指定行列的值
//结果在szBuffer中
LRESULT MyListView::GetListViewItem(DWORD dwLine, DWORD dwCol, CHAR* _lpszText, int nDefaultMaxBuff)
{
	ListView_GetItemText(hModuleTable, dwLine, dwCol, _lpszText, nDefaultMaxBuff);
	return 0;
	//LV_ITEM stLVI;
	//RtlZeroMemory(&stLVI, sizeof(LV_ITEM));
	//RtlZeroMemory(_lpszText, nDefaultMaxBuff);
	//stLVI.cchTextMax = nDefaultMaxBuff;
	//stLVI.mask = LVIF_TEXT;
	//stLVI.iSubItem = dwCol;
	//return SendMessage(hWinView, LVM_GETITEMTEXT, dwLine, (LPARAM)&stLVI);
}


//初始化结果表格

void MyListView::ClearResultView()
{
	ListViewClear();

	typeIter iter = columnName.Begin();
	int i = 0;
	while (iter != columnName.End())
	{
		const char* szColumnName = iter.GetKey();
		int column = iter.GetData();
		ListViewAddColumn(column, 200, szColumnName);
		iter++;
	}

}

void MyListView::AddLine(AddRowInfo& stInfo)
{
	dwCount = ListViewSetItem(dwCount, -1, stInfo.content);
	
	int columnIndex = 0;
	TCHAR numberBuffer[256];
	ZeroMemory(numberBuffer, 256);
	wsprintf(numberBuffer, TEXT("编号索引：%d"), stInfo.number);
	ListViewSetItem(dwCount, columnIndex, numberBuffer);
	columnIndex++;
	ListViewSetItem(dwCount, columnIndex, stInfo.content);
}


void MyListView::SetParent(HWND hParent)
{
	hWinParent = hParent;
}

HWND MyListView::GetParent()
{
	return hWinParent;
}

DWORD MyListView::GetCount()
{
	return dwCount;

}

HWND MyListView::GetModuleHWND()
{
	return hModuleTable;
}