#include "stdafx.h"
#include "SingleFileErrorShow.h"

MyListView* g_pSingleErrorFileLeftView = NULL;

char* strViewColumnName[] =
{
	"编号",
	"错误号",
	NULL,
};


DWORD WINAPI SingleDlgThreadProc(LPVOID lpParam)
{
	SingleFileUIThreadInfo* pInfo = (SingleFileUIThreadInfo*)lpParam;
	if (NULL == pInfo->strBuff)
	{
		return 0;
	}

	HWND openDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_SINGLEFILE), pInfo->hParent, (DLGPROC)SingleDlgProc);

	OpenFileList stFileData;
	stFileData.hWindowHandle = openDialog;

	strPodAllOpenList.Add(pInfo->strBuff, stFileData);

	ShowWindow(openDialog, SW_SHOW);

	delete pInfo->strBuff;
	return 0;
}

void InitSingleFileMainDlg(HWND hWnd)
{
	HWND hLeftList = GetDlgItem(hWnd, IDC_LEFTLIST);
	g_pSingleErrorFileLeftView = new MyListView(strViewColumnName, hLeftList);

	//定义表格外观
	SendMessage(hLeftList, LVM_SETEXTENDEDLISTVIEWSTYLE, \
		0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ShowWindow(hLeftList, SW_SHOW);
	g_pSingleErrorFileLeftView->ClearResultView();
}

INT CALLBACK SingleDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	LPNMHDR lpNMHDR = NULL;
	switch (message)
	{
	case WM_INITDIALOG:
		InitSingleFileMainDlg(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
