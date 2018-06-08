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
	"���ִ���",
	"���ֹؼ���",
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

	//���������
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
	lstrcpy(stFormat.szFaceName, TEXT("����"));
	SendMessage(hRightList, EM_SETCHARFORMAT, 0, (LPARAM)&stFormat);
	SendMessage(hRightList, EM_EXLIMITTEXT, 0, -1);

	g_pSingleViewContainer->hRightDlg = hRightList;
}


//һ��Ҫע���ڴ���ͷ����⡣
INT CALLBACK SingleDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int index = 0;
	LPNMHDR lpNMHDR = NULL;
	switch (message)
	{
	case WM_NOTIFY:
		//listView˫���¼���Ӧ
		lpNMHDR = (LPNMHDR)lParam;
		switch (lpNMHDR->code)
		{
		case NM_DBLCLK:
			if (lpNMHDR->idFrom == IDC_LEFTLIST)
			{
				MyDebugOutPut("�����%d\n", lpNMHDR->hwndFrom);
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
				Handle_RIGHT_DLG_INIT_INFO((OpenFileList*)lParam);
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

	g_pSingleViewContainer->m_AnalysisModule->Init(pOpenFile->strFilePath);
	g_pSingleViewContainer->m_AnalysisModule->SetWantKey(keyName);

	//��ʼstart������ʱ�򣬵�������һ���̡߳�Ȼ�󽫸��µ���ϢPUSH�������ϣ���ֹ������
	g_pSingleViewContainer->m_ThreadOpInfo.hThread = CreateThread(NULL, 0, SingleFileAnalysisThreadOp, 
		NULL, 0, &g_pSingleViewContainer->m_ThreadOpInfo.dwThreadID);

	delete pOpenFile->strFilePath;
	delete pOpenFile;
}


//���ұߵ�editbox���涪����
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
	OpenFileList* pCurrentFileList = g_pSingleViewContainer->m_CurrentFileList;

	TStringPod<char, ShowKeyInfo*>::iterator iter = g_pSingleViewContainer->m_PodAlreadyInList.Find(pCurrentFileList->strFilePath);

	if (iter == g_pSingleViewContainer->m_PodAlreadyInList.End())
	{
		MyDebugOutPut("Handle_RIGHT_DLG_INIT_INFO û���ҵ�Ŀ��\n");
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
		MessageBox(NULL, TEXT("index�����±��0��ʼ"), TEXT("index�����±��0��ʼ"), MB_OK);
		return;
	}

	ShowKeyInfoCell* pShowCell = (*pArrayPod)[nIndex];

	CHAR* pCurrentFilePointer = pShowCell->fileStartPointer;

	int nSize = g_pSingleViewContainer->m_AnalysisModule->GetSize();
	CHAR* pFileStartPointer = (CHAR*)g_pSingleViewContainer->m_AnalysisModule->GetStartSourceMemory();

	CHAR* pFileEndPointer = pFileStartPointer + nSize - 1;


	int outPutLength = pFileEndPointer - pCurrentFilePointer + 1;

	outPutLength = outPutLength >= ShowOutDefaultLength ? ShowOutDefaultLength : outPutLength;

	AppendInfoMation(targetView, pCurrentFilePointer, outPutLength);


}

void Handle_RIGHT_DLG_INIT_INFO(OpenFileList* pOpenFile)
{
	HWND hFromWhichWindow = pOpenFile->hWindowHandle;
	char* strPath = pOpenFile->strFilePath;

	TStringPod<char, ShowKeyInfo*>::iterator iter = g_pSingleViewContainer->m_PodAlreadyInList.Find(strPath);
	if (iter == g_pSingleViewContainer->m_PodAlreadyInList.End())
	{
		MyDebugOutPut("Handle_RIGHT_DLG_INIT_INFO û���ҵ�Ŀ��\n");
		MyDebugOutPut(strPath);
		return;
	}

	ShowKeyInfo* pShowKeyInfo = iter.GetData();

	MyDebugOutPut("����Ϊ: %d\n", pShowKeyInfo->nShowTime);
	MyDebugOutPut("����Ϊ: \n\n");

	TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowKeyInfo->pArrayPod;


	for (size_t i = 0; i < pArrayPod->size(); ++i)
	{
		ShowKeyInfoCell* pShowCell = (*pArrayPod)[i];
		MyDebugOutPut("��%d���ط����ļ���ʼλ��Ϊ%d\n", i, pShowCell->fileStartPos);
	}

	ShowCertainIndexLog(GetParent(hFromWhichWindow));

}


DWORD WINAPI SingleFileAnalysisThreadOp(LPVOID lpParam)
{
	g_pSingleViewContainer->m_AnalysisModule->AnalysisStart();
	TStringPod<char, ShowKeyInfo*>::iterator iter = g_pSingleViewContainer->m_PodAlreadyInList.Begin();


	AddRowInfo stRowInfo;
	stRowInfo.clear();

	while (iter != g_pSingleViewContainer->m_PodAlreadyInList.End())
	{
		ShowKeyInfo* pShowKeyInfo = iter.GetData();
		char* buff = (char*)iter.GetKey();

		stRowInfo.push_back(util_int_as_string(pShowKeyInfo->nShowTime).c_str());
		stRowInfo.push_back(buff);
		g_pSingleErrorFileLeftView->AddLine(stRowInfo);
		stRowInfo.clear();
		iter++;
	}

	g_pSingleViewContainer->m_ThreadOpInfo.dwThreadID = NULL;
	g_pSingleViewContainer->m_ThreadOpInfo.hThread = NULL;
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

	g_pSingleErrorFileLeftView->GetListViewItem(lpInfo->iItem, 1, resultBuffer, MAX_PATH - 1);

	if (resultBuffer[0] == '\0')
	{
		MyDebugOutPut("HandleDoubleClickRightDlgʧ��\n");
		MyDebugOutPut("��Ϊ%d\n", lpInfo->iItem);
		return 0;
	}

	TStringPod<char, OpenFileList>::iterator iter = g_pSingleViewContainer->strPodAllOpenList.Find(resultBuffer);
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

	g_pSingleViewContainer->m_CurrentFileList = pStFileData;

	PostMessage(hWnd, WM_MY_MESSAGE, RIGHT_DLG_INIT_INFO, (LPARAM)pStFileData);


	//SingleFileUIThreadInfo* pData = new SingleFileUIThreadInfo;
	//pData->hParent = hWnd;
	//pData->strBuff = resultBuffer;

	////����һ��������dialog�������е�error���б��
	//DWORD nthreadID;
	//HandleStartLogOpThreadHandle = CreateThread(NULL, 0, SingleDlgThreadProc, pData, 0, &nthreadID);
	return 0;
}