#include "stdafx.h"
#include "Shlobj.h"
#include "shlwapi.h"
#include "AnalysisLog.h"
#include "SingleFileErrorShow.h"
#include <map>
#include "GetKeyWorld.h"
#include "util_func.h"

/************************************************************************/
/* 对外仅输出一个函数HandleOpenFileOp                                                                     */
/************************************************************************/


TCHAR szOutPutPathName[MAX_PATH];

/*
	感觉其实应该把这个mainView作为抽象容器，view等都是这个容器里面的一个物件，现在只能孤零零的把这个CPP文件作为容器来看待。
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

//文件的名字->第几个的映射
FILE_NAME_ID_MAP stNameIDMap;

//比较文件的后缀名，只有txt才会收录
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
		MyDebugOutPut(TEXT("当前遍历的结果为：%s \t\tvalue: %d\n"), name, number);
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
		MyDebugOutPut(TEXT("居然已经在name_map中找到了？ %s\n"), szFilePath);
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
		printf("创建内存映射文件失败\n");
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
		//wsprintf(outPutBuffer, TEXT("总共找到 %d 个文件夹, %d 个文件, 一共 %lu字节"), dwFolderCount, dwFileCount, dwFileTotalSize);
		//OutputDebugString(outPutBuffer);
		//MyDebugOutPut(TEXT("总共找到 %d 个文件夹, %d 个文件, 一共 %lu字节"), dwFolderCount, dwFileCount, dwFileTotalSize);
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
	//根据keyName，和上面的文件信息，进行ERROR输出。

	TStringPod<char, int> podAlreadyInList;
	podAlreadyInList.Clear();

	AnalysisBehaviorLine_Func01* behavior = new AnalysisBehaviorLine_Func01();
	behavior->podAlreadyInList = &podAlreadyInList;

	AnalysisLogModule* analysisModule = new AnalysisLogModule();
	
	FILE_NAME_ID_MAP_CONST_ITERATOR const_iter = stNameIDMap.begin();


	MyFile outPutLog;
	if (!outPutLog.Init(szOutPutPathName))
	{
		MyDebugOutPut(TEXT("输出文件创建失败"));
		MessageBox(NULL, TEXT("输出文件创建失败"), TEXT("搞事情啊"), MB_OK);
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
	MessageBox(NULL, TEXT("Finish"),TEXT("提取完成"), MB_OK);
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
	ofn.nMaxFile = sizeof(szOutPutPathName);//存放用户选择文件的 路径及文件名 缓冲区  
	ofn.lpstrTitle = TEXT("选择输出文件");//选择文件对话框标题  
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	ofn.lpstrInitialDir = szCurDir;//设置对话框显示的初始目录  
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;//如果需要选择多个文件 则必须带有  OFN_ALLOWMULTISELECT标志  
	_tprintf(TEXT("select file/n"));
	BOOL bOk = GetOpenFileName(&ofn);//调用对话框打开文件  
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
		MyDebugOutPut("HandleDoubleClickListView失败\n");
		MyDebugOutPut("行为%d\n", lpInfo->iItem);
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

	////创建一个单独的dialog，将所有的error进行标红
	//HandleStartLogOpThreadHandle = CreateThread(NULL, 0, SingleDlgThreadProc, pData, 0, &pData->dwThreadID);
	return 0;
}