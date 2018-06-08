#ifndef _LIST_VIEW_RELATED_H_
#define _LIST_VIEW_RELATED_H_
#include "Commctrl.h"
#include "StringPod.h"
#include "ArrayPod.h"
#include "FastStr.h"

typedef TArrayPod<fast_doubleString, 10> AddRowInfo;


class MyListView
{
public:
	MyListView(char* colunmName[], HWND hModuleTable);
	LRESULT ListViewAddColumn(int nColumn, int nWidth, const CHAR* lpszHead);
	LRESULT ListViewSetItem(int nItem, int nSubItem, const TCHAR* lpszText);
	void ListViewClear();
	LRESULT GetListViewItem(DWORD dwLine, DWORD dwCol, CHAR* _lpszText, int nDefaultMaxBuff = MAX_PATH);
	void ClearResultView();
	void AddLine(AddRowInfo& stInfo);
	void SetParent(HWND);
	HWND GetParent();
	DWORD GetCount();
	HWND GetModuleHWND();
private:
	HWND hWinParent;
	TArrayPod<fast_string, 10> columnName;
	HWND hModuleTable;
	DWORD dwCount;
	LV_COLUMN stLVC;
	LV_ITEM stLVI;
};


extern HWND hModuleTable;

#endif