#include "stdafx.h"
#include "SingleFileErrorShow.h"
#include "util_func.h"
#include "Richedit.h"

void ReleaseViewRelatedInfo(HWND hWnd)
{
	if (NULL != GetSingleFileContainer(hWnd))
	{
		delete GetSingleFileContainer(hWnd);
		RemoveHWNDFromManager(hWnd);
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

	if (NULL != m_LeftListView)
	{
		delete m_LeftListView;
		m_LeftListView = NULL;
	}

	TStringPod<char, ShowKeyInfo*>::iterator iter = m_PodAlreadyInList.Begin();

	while (iter != m_PodAlreadyInList.End())
	{
		ShowKeyInfo* pShowKeyInfo = iter.GetData();
		TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowKeyInfo->pArrayPod;

		for (size_t i = 0; i < pArrayPod->size(); ++i)
		{
			ShowKeyInfoCell* pShowCell = (*pArrayPod)[i];
			delete pShowCell;
		}
		delete pArrayPod;
		iter++;
	}


	TStringPod<char, OpenFileList>::iterator iter2 = strPodAllOpenList.Begin();

	while (iter2 != strPodAllOpenList.End())
	{
		OpenFileList& data = iter2.GetData();
		if (NULL != data.strFilePath)
		{
			delete data.strFilePath;
		}
	}

	m_PodAlreadyInList.Clear();
	strPodAllOpenList.Clear();
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

void InitSingleFileMainDlg(HWND hWnd)
{
	HWND hLeftList = GetDlgItem(hWnd, IDC_LEFTLIST);

	MyListView* pSingleErrorFileLeftView = new MyListView(strViewColumnName, hLeftList);

	SingleViewContainer* pSingleViewContainer = new SingleViewContainer();
	AddContailer2Manager(hWnd, pSingleViewContainer);

	pSingleViewContainer->m_LeftListView = pSingleErrorFileLeftView;

	//定义表格外观
	SendMessage(hLeftList, LVM_SETEXTENDEDLISTVIEWSTYLE, \
		0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ShowWindow(hLeftList, SW_SHOW);
	pSingleErrorFileLeftView->ClearResultView();

	pSingleViewContainer->hMainDlg = hWnd;
	pSingleViewContainer->hLeftDlg = hLeftList;
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

	GetSingleFileContainer(hWnd)->hRightDlg = hRightList;
}


//一定要注意内存的释放问题。
INT CALLBACK SingleDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int index = 0;
	LPNMHDR lpNMHDR = NULL;
	switch (message)
	{
	case WM_NOTIFY:
		//listView双击事件响应
		lpNMHDR = (LPNMHDR)lParam;
		switch (lpNMHDR->code)
		{
		case NM_DBLCLK:
			if (lpNMHDR->idFrom == IDC_LEFTLIST)
			{
				MyDebugOutPut("点击了%d\n", lpNMHDR->hwndFrom);
			}
			HandleDoubleClickRightDlg(hWnd, (LPNMITEMACTIVATE)lParam);
		}
		break;
	case WM_MY_MESSAGE:
		switch (wParam)
		{
			case DLG_INIT_INFO:
				Handle_DLG_INIT_INFO(hWnd, (OpenFileList*)lParam);
				break;
			case RIGHT_DLG_INIT_INFO:
				Handle_RIGHT_DLG_INIT_INFO(hWnd, (OpenFileList*)lParam);
				break;
		}
		break;
	case WM_INITDIALOG:
		InitSingleFileMainDlg(hWnd);
		break;
	case WM_CLOSE:
		ReleaseViewRelatedInfo(hWnd);
		DestroyWindow(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_SURE:
			index = GetDlgItemInt(hWnd, IDC_EDIT1, NULL, TRUE);
			ShowCertainIndexLog(hWnd, index);
			break;
		case IDC_NEXT:
			index = GetDlgItemInt(hWnd, IDC_EDIT1, NULL, TRUE);
			index = index + 1;
			SetDlgItemInt(hWnd, IDC_EDIT1, index, TRUE);
			ShowCertainIndexLog(hWnd, index);
		}
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

	GetSingleFileContainer(hWnd)->m_AnalysisModule->Init(pOpenFile->strFilePath);
	GetSingleFileContainer(hWnd)->m_AnalysisModule->SetWantKey(keyName);

	//开始start分析的时候，单独开启一个线程。然后将更新的消息PUSH到界面上，防止卡死。
	GetSingleFileContainer(hWnd)->m_ThreadOpInfo.hThread = CreateThread(NULL, 0, SingleFileAnalysisThreadOp,
		hWnd, 0, &GetSingleFileContainer(hWnd)->m_ThreadOpInfo.dwThreadID);

	delete pOpenFile->strFilePath;
	delete pOpenFile;
}


//往右边的editbox里面丢数据
void AppendInfoMation(HWND hRichEdit, char* szData, int nLength)
{
	CHARRANGE stCharRange;
	//int nLength = GetWindowTextLength(hRichEdit);
	stCharRange.cpMax = nLength;
	stCharRange.cpMin = nLength;
	SendMessage(hRichEdit, EM_EXSETSEL, 0, (LPARAM)&stCharRange);
	//SendMessage(hRichEdit, EM_REPLACESEL, FALSE, (LPARAM)szData);
	SendMessage(hRichEdit, WM_SETTEXT, 0, (LPARAM)szData);
}


void ShowCertainIndexLog(HWND hWnd, int nIndex)
{
	OpenFileList* pCurrentFileList = GetSingleFileContainer(hWnd)->m_CurrentFileList;

	TStringPod<char, ShowKeyInfo*>::iterator iter = GetSingleFileContainer(hWnd)->m_PodAlreadyInList.Find(pCurrentFileList->strFilePath);

	if (iter == GetSingleFileContainer(hWnd)->m_PodAlreadyInList.End())
	{
		MyDebugOutPut("Handle_RIGHT_DLG_INIT_INFO 没有找到目标\n");
		MyDebugOutPut(pCurrentFileList->strFilePath);
		return;
	}

	ShowKeyInfo* pShowKeyInfo = iter.GetData();
	TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowKeyInfo->pArrayPod;

	if (nIndex < 0)
	{
		nIndex = 0;
	}

	HWND targetView = GetDlgItem(hWnd, IDC_RIGHTVIEW);
	if (nIndex >= (int)pArrayPod->size())
	{
		MessageBox(NULL, TEXT("index过大，下标从0开始"), TEXT("index过大，下标从0开始"), MB_OK);
		return;
	}

	ShowKeyInfoCell* pShowCell = (*pArrayPod)[nIndex];

	CHAR* pCurrentFilePointer = pShowCell->fileStartPointer;

	int nSize = GetSingleFileContainer(hWnd)->m_AnalysisModule->GetSize();
	CHAR* pFileStartPointer = (CHAR*)GetSingleFileContainer(hWnd)->m_AnalysisModule->GetStartSourceMemory();

	CHAR* pFileEndPointer = pFileStartPointer + nSize - 1;


	int outPutLength = pFileEndPointer - pCurrentFilePointer + 1;

	outPutLength = outPutLength >= ShowOutDefaultLength ? ShowOutDefaultLength : outPutLength;

	AppendInfoMation(targetView, pCurrentFilePointer, outPutLength);


}

void Handle_RIGHT_DLG_INIT_INFO(HWND hWnd, OpenFileList* pOpenFile)
{
	HWND hFromWhichWindow = pOpenFile->hWindowHandle;
	char* strPath = pOpenFile->strFilePath;

	TStringPod<char, ShowKeyInfo*>::iterator iter = GetSingleFileContainer(hWnd)->m_PodAlreadyInList.Find(strPath);
	if (iter == GetSingleFileContainer(hWnd)->m_PodAlreadyInList.End())
	{
		MyDebugOutPut("Handle_RIGHT_DLG_INIT_INFO 没有找到目标\n");
		MyDebugOutPut(strPath);
		return;
	}

	ShowKeyInfo* pShowKeyInfo = iter.GetData();

	MyDebugOutPut("次数为: %d\n", pShowKeyInfo->nShowTime);
	MyDebugOutPut("内容为: \n\n");

	TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowKeyInfo->pArrayPod;


	for (size_t i = 0; i < pArrayPod->size(); ++i)
	{
		ShowKeyInfoCell* pShowCell = (*pArrayPod)[i];
		MyDebugOutPut("第%d个地方的文件起始位置为%d\n", i, pShowCell->fileStartPos);
	}

	ShowCertainIndexLog(GetParent(hFromWhichWindow));

}


DWORD WINAPI SingleFileAnalysisThreadOp(LPVOID lpParam)
{
	HWND hWnd = (HWND)lpParam;
	GetSingleFileContainer(hWnd)->m_AnalysisModule->AnalysisStart();
	TStringPod<char, ShowKeyInfo*>::iterator iter = GetSingleFileContainer(hWnd)->m_PodAlreadyInList.Begin();

	AddRowInfo stRowInfo;
	stRowInfo.clear();

	while (iter != GetSingleFileContainer(hWnd)->m_PodAlreadyInList.End())
	{
		ShowKeyInfo* pShowKeyInfo = iter.GetData();
		char* buff = (char*)iter.GetKey();

		stRowInfo.push_back(util_int_as_string(pShowKeyInfo->nShowTime).c_str());
		stRowInfo.push_back(buff);
		GetSingleFileContainer(hWnd)->m_LeftListView->AddLine(stRowInfo);
		stRowInfo.clear();
		iter++;
	}

	GetSingleFileContainer(hWnd)->m_ThreadOpInfo.dwThreadID = NULL;
	GetSingleFileContainer(hWnd)->m_ThreadOpInfo.hThread = NULL;
	return 0;

}

LRESULT HandleDoubleClickRightDlg(HWND hWnd, LPNMITEMACTIVATE lpInfo)
{
	if (NULL == lpInfo)
	{
		return 0;
	}

	CHAR* resultBuffer = new CHAR[MAX_PATH];
	ZeroMemory(resultBuffer, MAX_PATH);

	GetSingleFileContainer(hWnd)->m_LeftListView->GetListViewItem(lpInfo->iItem, 1, resultBuffer, MAX_PATH - 1);

	if (resultBuffer[0] == '\0')
	{
		MyDebugOutPut("HandleDoubleClickRightDlg失败\n");
		MyDebugOutPut("行为%d\n", lpInfo->iItem);
		return 0;
	}

	TStringPod<char, OpenFileList>::iterator iter = GetSingleFileContainer(hWnd)->strPodAllOpenList.Find(resultBuffer);
	//if (iter != strPodAllOpenList.End())
	//{
	//	OpenFileList stData = iter.GetData();
	//	if (IsWindow(stData.hWindowHandle))
	//	{
	//		BOOL result = SetWindowPos(stData.hWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	//		if (result)
	//		{
	//			return 0;
	//		}
	//	}
	//	else
	//	{
	//		strPodAllOpenList.Remove(iter.GetKey());
	//	}
	//}

	OpenFileList* pStFileData = new OpenFileList();

	pStFileData->hWindowHandle = lpInfo->hdr.hwndFrom;
	pStFileData->strFilePath = resultBuffer;

	strPodAllOpenList.Add(resultBuffer, *pStFileData);

	GetSingleFileContainer(hWnd)->m_CurrentFileList = pStFileData;

	PostMessage(hWnd, WM_MY_MESSAGE, RIGHT_DLG_INIT_INFO, (LPARAM)pStFileData);


	//SingleFileUIThreadInfo* pData = new SingleFileUIThreadInfo;
	//pData->hParent = hWnd;
	//pData->strBuff = resultBuffer;

	////创建一个单独的dialog，将所有的error进行标红
	//DWORD nthreadID;
	//HandleStartLogOpThreadHandle = CreateThread(NULL, 0, SingleDlgThreadProc, pData, 0, &nthreadID);
	return 0;
}



DWORD WINAPI SingleDlgThreadProc(LPVOID lpParam)
{
	SingleFileUIThreadInfo* pInfo = (SingleFileUIThreadInfo*)lpParam;
	if (NULL == pInfo->strBuff)
	{
		return 0;
	}


	HWND openDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_SINGLEFILE), pInfo->hParent, (DLGPROC)SingleDlgProc);


	pInfo->pStOpenFileList->hWindowHandle = openDialog;

	ShowWindow(openDialog, SW_SHOW);

	PostThreadMessage(pInfo->dwThreadID, WM_MY_MESSAGE, DLG_INIT_INFO, (LPARAM)(pInfo->pStOpenFileList));
	//PostMessage(openDialog, WM_MY_MESSAGE, DLG_INIT_INFO, (LPARAM)(pInfo->pStOpenFileList));

	delete pInfo;
	return 0;
}