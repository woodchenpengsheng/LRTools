#include "stdafx.h"
#include "SingleFileErrorShow.h"
#include "util_func.h"
#include "Richedit.h"

MyListView* g_pSingleErrorFileLeftView = NULL;
SingleViewContainer* g_pSingleViewContainer = NULL;

void ReleaseViewRelatedInfo()
{
	if (NULL != g_pSingleErrorFileLeftView)
	{
		delete g_pSingleErrorFileLeftView;
		g_pSingleErrorFileLeftView = NULL;
	}

	if (NULL != g_pSingleViewContainer)
	{
		delete g_pSingleViewContainer;
		g_pSingleViewContainer = NULL;
	}

}

SingleViewContainer::~SingleViewContainer()
{
	if (NULL != m_AnalysisModule)
	{
		delete m_AnalysisModule;
		m_AnalysisModule = NULL;
	}

	if (NULL != m_BehaviorModule)
	{
		delete m_BehaviorModule;
		m_BehaviorModule = NULL;
	}
}

SingleViewContainer::SingleViewContainer()
{
	m_ThreadOpInfo.dwThreadID = 0;
	m_ThreadOpInfo.hThread = NULL;
	m_PodAlreadyInList.Clear();

	AnalysisBehaviorLine_Func02* behavior = new AnalysisBehaviorLine_Func02();
	behavior->m_SingleFileList = &m_PodAlreadyInList;

	m_AnalysisModule = new AnalysisLogModule();;
	m_BehaviorModule = behavior;
	m_AnalysisModule->SetLineOp(m_BehaviorModule);
}


char* strViewColumnName[] =
{
	"出现次数",
	"部分关键码",
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
	g_pSingleViewContainer = new SingleViewContainer();

	//定义表格外观
	SendMessage(hLeftList, LVM_SETEXTENDEDLISTVIEWSTYLE, \
		0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ShowWindow(hLeftList, SW_SHOW);
	g_pSingleErrorFileLeftView->ClearResultView();

	g_pSingleViewContainer->hMainDlg = hWnd;
	g_pSingleViewContainer->hLeftDlg = hLeftList;
}

void InitSingleFileRightDlg(HWND hWnd)
{
	HWND hRightList = GetDlgItem(hWnd, IDC_RIGHTVIEW);

	SendMessage(hRightList, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
	CHARFORMAT stFormat;
	ZeroMemory(&stFormat, sizeof(CHARFORMAT));
	stFormat.cbSize = sizeof(stFormat);
	stFormat.yHeight = 9 * 20;
	stFormat.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD;
	lstrcpy(stFormat.szFaceName, TEXT("宋体"));
	SendMessage(hRightList, EM_SETCHARFORMAT, 0, (LPARAM)&stFormat);
	SendMessage(hRightList, EM_EXLIMITTEXT, 0, -1);

	g_pSingleViewContainer->hRightDlg = hRightList;
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
		ReleaseViewRelatedInfo();
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

	g_pSingleViewContainer->m_AnalysisModule->Init(pOpenFile->strFilePath);
	g_pSingleViewContainer->m_AnalysisModule->SetWantKey(keyName);

	//开始start分析的时候，单独开启一个线程。然后将更新的消息PUSH到界面上，防止卡死。
	g_pSingleViewContainer->m_ThreadOpInfo.hThread = CreateThread(NULL, 0, SingleFileAnalysisThreadOp, 
		NULL, 0, &g_pSingleViewContainer->m_ThreadOpInfo.dwThreadID);

	delete pOpenFile->strFilePath;
	delete pOpenFile;
}


DWORD WINAPI SingleFileAnalysisThreadOp(LPVOID lpParam)
{
	g_pSingleViewContainer->m_AnalysisModule->AnalysisStart();
	g_pSingleViewContainer->m_AnalysisModule->Clear();
	TStringPod<char, ShowKeyInfo*>::iterator iter = g_pSingleViewContainer->m_PodAlreadyInList.Begin();


	AddRowInfo stRowInfo;
	stRowInfo.clear();

	while (iter != g_pSingleViewContainer->m_PodAlreadyInList.End())
	{
		int nTimes = iter.GetData();
		char* buff = (char*)iter.GetKey();

		stRowInfo.push_back(util_int_as_string(nTimes).c_str());
		stRowInfo.push_back(buff);
		g_pSingleErrorFileLeftView->AddLine(stRowInfo);
		stRowInfo.clear();
		iter++;
	}

	g_pSingleViewContainer->m_ThreadOpInfo.dwThreadID = NULL;
	g_pSingleViewContainer->m_ThreadOpInfo.hThread = NULL;
	return 0;

}