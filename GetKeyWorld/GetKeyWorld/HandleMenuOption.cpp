#include "stdafx.h"
#include "Shlobj.h"
#include "shlwapi.h"
#include "AnalysisLog.h"
#include "SingleFileErrorShow.h"
#include <map>
#include "GetKeyWorld.h"
#include "util_func.h"

/************************************************************************/
/* ��������һ������HandleOpenFileOp                                                                     */
/************************************************************************/


TCHAR szOutPutPathName[MAX_PATH];

/*
	�о���ʵӦ�ð����mainView��Ϊ����������view�ȶ���������������һ�����������ֻ�ܹ�����İ����CPP�ļ���Ϊ������������
*/
TStringPod<char, OpenFileList> strPodAllOpenList;

struct cmp_str
{
	bool operator()(TCHAR const *a, TCHAR const *b)
	{
		return lstrcmp(a, b) < 0;
	}
};

typedef std::map<TCHAR*, UINT, cmp_str> FILE_NAME_ID_MAP;
typedef FILE_NAME_ID_MAP::iterator FILE_NAME_ID_MAP_ITERATOR;
typedef FILE_NAME_ID_MAP::const_iterator FILE_NAME_ID_MAP_CONST_ITERATOR;

//�ļ�������->�ڼ�����ӳ��
FILE_NAME_ID_MAP stNameIDMap;

//�Ƚ��ļ��ĺ�׺����ֻ��txt�Ż���¼
BOOL ExtensionCheck(const TCHAR* szExtension)
{
	if (lstrcmp(szExtension, TEXT(".log")) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

void OutPutMapData(const FILE_NAME_ID_MAP& szOutPutMap)
{
	FILE_NAME_ID_MAP_CONST_ITERATOR const_iter = szOutPutMap.begin();
	for (; const_iter != szOutPutMap.end(); const_iter++)
	{
		TCHAR* name = const_iter->first;
		UINT number = const_iter->second;
		MyDebugOutPut(TEXT("��ǰ�����Ľ��Ϊ��%s \t\tvalue: %d\n"), name, number);
	}
}

void OutPutView(const FILE_NAME_ID_MAP& szOutPutMap)
{
	AddRowInfo stRowInfo;
	stRowInfo.clear();

	FILE_NAME_ID_MAP_CONST_ITERATOR const_iter = szOutPutMap.begin();
	for (; const_iter != szOutPutMap.end(); const_iter++)
	{
		CHAR* name = const_iter->first;
		UINT number = const_iter->second;
		stRowInfo.push_back(util_int_as_string(number).c_str());
		stRowInfo.push_back(name);
		g_pMainListView->AddLine(stRowInfo);
		stRowInfo.clear();
	}
}

BOOL ProcessFile_01(TCHAR* szFilePath)
{
	TCHAR* szExtensionCheck = PathFindExtension(szFilePath);
	if (!ExtensionCheck(szExtensionCheck))
	{
		return  FALSE;
	}
	FILE_NAME_ID_MAP_ITERATOR iter = stNameIDMap.find(szFilePath);
	if (iter != stNameIDMap.end())
	{
		MyDebugOutPut(TEXT("��Ȼ�Ѿ���name_map���ҵ��ˣ� %s\n"), szFilePath);
		return FALSE;
	}
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("ProcessFile Error");
		return FALSE;
	}
	HANDLE hFileMap = CreateFileMapping(hFile, NULL,
		PAGE_READONLY, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		printf("�����ڴ�ӳ���ļ�ʧ��\n");
		return FALSE;
	}
	void* lpMemory = MapViewOfFile(hFileMap,
		FILE_MAP_READ, 0, 0, 0);

	UnmapViewOfFile(lpMemory);
	CloseHandle(hFileMap);
	CloseHandle(hFile);

	dwFileCountFirst = dwFileCountFirst + 1;

	UINT uLength = lstrlen(szFilePath);
	UINT uSize = (uLength + 1) * sizeof(TCHAR);
	TCHAR* szTempBuffer = (TCHAR*)malloc(uSize);
	RtlZeroMemory(szTempBuffer, uSize);
	lstrcpy(szTempBuffer, szFilePath);

	stNameIDMap.insert(FILE_NAME_ID_MAP::value_type(szTempBuffer, dwFileCountFirst));
	return TRUE;
}

LRESULT HandleOpenFileOp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szPath[MAX_PATH];
	RtlZeroMemory(szPath, MAX_PATH);
	if (BrowserFolder(NULL, szPath))
	{
		//MessageBox(NULL, szPath, szSelect, MB_OK);
		//return 0;
		FindFile(szPath, ProcessFile_01);
		OutPutMapData(stNameIDMap);
		OutPutView(stNameIDMap);
		//FindFile(szPath, ProcessFile);
		//ZeroMemory(outPutBuffer, OUT_PUT_BUFFET_LENGTH);
		//wsprintf(outPutBuffer, TEXT("�ܹ��ҵ� %d ���ļ���, %d ���ļ�, һ�� %lu�ֽ�"), dwFolderCount, dwFileCount, dwFileTotalSize);
		//OutputDebugString(outPutBuffer);
		//MyDebugOutPut(TEXT("�ܹ��ҵ� %d ���ļ���, %d ���ļ�, һ�� %lu�ֽ�"), dwFolderCount, dwFileCount, dwFileTotalSize);
		return 0;
	}
	else
	{
		MessageBox(NULL, szNoSelect, NULL, MB_OK);
		return -1;
	}
}

LRESULT HandleStartLogOp(HWND hWnd, int ItemId)
{
	TCHAR keyName[MAX_PATH];
	ZeroMemory(keyName, MAX_PATH);
	GetDlgItemText(hWnd, ItemId, keyName, MAX_PATH - 1);
	//����keyName����������ļ���Ϣ������ERROR�����

	TStringPod<char, int> podAlreadyInList;
	podAlreadyInList.Clear();

	AnalysisBehaviorLine_Func01* behavior = new AnalysisBehaviorLine_Func01();
	behavior->podAlreadyInList = &podAlreadyInList;

	AnalysisLogModule* analysisModule = new AnalysisLogModule();
	
	FILE_NAME_ID_MAP_CONST_ITERATOR const_iter = stNameIDMap.begin();


	MyFile outPutLog;
	if (!outPutLog.Init(szOutPutPathName))
	{
		MyDebugOutPut(TEXT("����ļ�����ʧ��"));
		MessageBox(NULL, TEXT("����ļ�����ʧ��"), TEXT("�����鰡"), MB_OK);
		return FALSE;
	}

	for (; const_iter != stNameIDMap.end(); const_iter++)
	{
		TCHAR* name = const_iter->first;
		analysisModule->Init(name);
		analysisModule->SetLineOp(behavior);
		analysisModule->SetWantKey(keyName);
		analysisModule->AnalysisStart();
		analysisModule->Clear();
	}

	CHAR numberBuff[MAX_PATH];
	ZeroMemory(numberBuff, MAX_PATH);
	TStringPod<char, int>::const_iterator iterType = podAlreadyInList.Begin();

	for (; iterType != podAlreadyInList.End(); ++iterType)
	{
		const char* outPutKey = iterType.GetKey();
		int times = iterType.GetData();
		sprintf_s(numberBuff, "%d:", times);

		int length = strlen(numberBuff);
		outPutLog.Write(numberBuff, length);
		outPutLog.Write(outPutKey, strlen(outPutKey));
		numberBuff[length] = '\0';
	}

	podAlreadyInList.Clear();
	HandleStartLogOpThreadHandle = INVALID_HANDLE_VALUE;
	MessageBox(NULL, TEXT("Finish"),TEXT("��ȡ���"), MB_OK);
	delete analysisModule;
	delete behavior;
	
	return 1;
}


LRESULT HandleOutPutSaveLogFile(HWND hWnd)
{
	ZeroMemory(szOutPutPathName, MAX_PATH);
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
  
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = NULL;
	ofn.lpstrFile = szOutPutPathName;
	ofn.nMaxFile = sizeof(szOutPutPathName);//����û�ѡ���ļ��� ·�����ļ��� ������  
	ofn.lpstrTitle = TEXT("ѡ������ļ�");//ѡ���ļ��Ի������  
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	ofn.lpstrInitialDir = szCurDir;//���öԻ�����ʾ�ĳ�ʼĿ¼  
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;//�����Ҫѡ�����ļ� ��������  OFN_ALLOWMULTISELECT��־  
	_tprintf(TEXT("select file/n"));
	BOOL bOk = GetOpenFileName(&ofn);//���öԻ�����ļ�  
	return bOk;
}

DWORD WINAPI HandleStartLogOpThreadProc(LPVOID lpParam)
{
	StartLogOp* lpStartLp = (StartLogOp*)lpParam;
	return HandleStartLogOp(lpStartLp->hWnd, lpStartLp->keyNumber);
}

LRESULT HandleDoubleClickListView(HWND hWnd,LPNMITEMACTIVATE lpInfo)
{
	if (NULL == lpInfo)
	{
		return 0;
	}

	CHAR* resultBuffer = new CHAR[MAX_PATH];
	ZeroMemory(resultBuffer, MAX_PATH);

	g_pMainListView->GetListViewItem(lpInfo->iItem, 1, resultBuffer, MAX_PATH - 1);

	if (resultBuffer[0] == '\0')
	{
		MyDebugOutPut("HandleDoubleClickListViewʧ��\n");
		MyDebugOutPut("��Ϊ%d\n", lpInfo->iItem);
		return 0;
	}

	TStringPod<char, OpenFileList>::iterator iter = strPodAllOpenList.Find(resultBuffer);
	if (iter != strPodAllOpenList.End())
	{
		OpenFileList stData = iter.GetData();
		if (IsWindow(stData.hWindowHandle))
		{
			BOOL result = SetWindowPos(stData.hWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
			if (result)
			{
				return 0;
			}
		}
		else
		{
			strPodAllOpenList.Remove(iter.GetKey());
		}
	}

	HWND openDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_SINGLEFILE), hWnd, (DLGPROC)SingleDlgProc);

	OpenFileList* pStFileData = new OpenFileList();
	pStFileData->hWindowHandle = openDialog;
	pStFileData->strFilePath = resultBuffer;

	strPodAllOpenList.Add(resultBuffer, *pStFileData);
	ShowWindow(openDialog, SW_SHOW);


	PostMessage(openDialog, WM_MY_MESSAGE, DLG_INIT_INFO, (LPARAM)pStFileData);


	//SingleFileUIThreadInfo* pData = new SingleFileUIThreadInfo;
	//pData->hParent = hWnd;
	//pData->strBuff = resultBuffer;
	//pData->pStOpenFileList = pStFileData;

	////����һ��������dialog�������е�error���б��
	//HandleStartLogOpThreadHandle = CreateThread(NULL, 0, SingleDlgThreadProc, pData, 0, &pData->dwThreadID);
	return 0;
}