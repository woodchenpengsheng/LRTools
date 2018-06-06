#ifndef _LIST_VIEW_RELATED_H_
#define _LIST_VIEW_RELATED_H_
#include "Commctrl.h"
#include "StringPod.h"

typedef struct _AddRowInfo
{
	int number;
	TCHAR* content;
}AddRowInfo;

class MyListView
{
public:
	typedef TStringPod<char, int>::iterator typeIter;
	MyListView(char* colunmName[], HWND hModuleTable);
	LRESULT ListViewAddColumn(int nColumn, int nWidth, const CHAR* lpszHead);
	LRESULT ListViewSetItem(int nItem, int nSubItem, TCHAR* lpszText);
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
	TStringPod<char, int> columnName;
	HWND hModuleTable;
	DWORD dwCount;
	LV_COLUMN stLVC;
	LV_ITEM stLVI;
};


extern HWND hModuleTable;

#endif