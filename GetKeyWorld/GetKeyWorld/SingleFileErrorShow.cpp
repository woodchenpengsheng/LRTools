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

//一定要注意内存的释放问题。
INT CALLBACK SingleDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	LPNMHDR lpNMHDR = NULL;
	switch (message)
	{
	case WM_MY_MESSAGE:
		switch (wParam)
		{
			case DLG_INIT_INFO:
				Handle_DLG_INIT_INFO(hWnd, (OpenFileList*)lParam);
				break;
		}
		break;
	case WM_INITDIALOG:
		InitSingleFileMainDlg(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void Handle_DLG_INIT_INFO(HWND hWnd,OpenFileList* pOpenFile)
{
	TCHAR keyName[MAX_PATH];
	ZeroMemory(keyName, MAX_PATH);
	HWND hParentDlg = GetParent(hWnd);
	GetDlgItemText(hParentDlg, IDC_CURRENTKEY, keyName, MAX_PATH - 1);

	MyDebugOutPut("%s\n", pOpenFile->strFilePath);
	MyDebugOutPut("keyName: %s\n", keyName);

	AnalysisSingleFileModule* analysisModule = new AnalysisSingleFileModule();
	AnalysisBehaviorLine_Func01* lineFunc = new AnalysisBehaviorLine_Func01();

	analysisModule->Init(pOpenFile->strFilePath);
	analysisModule->SetWantKey(keyName);

	analysisModule->AnalysisStart();
	analysisModule->Clear();

	delete analysisModule;
	delete pOpenFile->strFilePath;
	delete pOpenFile;
	delete lineFunc;
}