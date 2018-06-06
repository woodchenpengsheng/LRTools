#include "stdafx.h"
#include "BasicUtil.h"
#include "Shlobj.h"
#include "shlwapi.h"

/************************************************************************/
/* �Զ����������                                                                     */
/************************************************************************/
#define  OUT_PUT_BUFFET_LENGTH 256
TCHAR outPutBuffer[OUT_PUT_BUFFET_LENGTH] = { 0 };

void MyDebugOutPut(const TCHAR* szFormat, ...)
{
	va_list pArgs;
	va_start(pArgs, szFormat);
	ZeroMemory(outPutBuffer, OUT_PUT_BUFFET_LENGTH);
	wvsprintf(outPutBuffer, szFormat, pArgs);
	OutputDebugString(outPutBuffer);
	va_end(pArgs);
}


/************************************************************************/
/* ���ѡ���ļ���                                                                   */
/************************************************************************/

DWORD BrowseFolderTmp;
TCHAR lpszTitle[] = TEXT("��ѡ��Ŀ¼");
TCHAR szSelect[] = TEXT("��ѡ���Ŀ¼");
TCHAR szNoSelect[] = TEXT("��������ȡ����");
TCHAR szFilter[] = TEXT("*.*");

UINT dwFolderCount = 0; //���ļ�����Ŀ
UINT dwFileCount = 0; //�ܹ��ж��ٸ��ļ�
ULONG dwFileTotalSize = 0; //�ļ��Ĵ�С

UINT dwFileCountFirst = 0;

TCHAR newFileExtension[] = TEXT(".txt");

int __stdcall BrowserFolderCallBack(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szBuffer[260];
	RtlZeroMemory(szBuffer, sizeof(szBuffer));
	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, BrowseFolderTmp);
	}
	else if (uMsg == BFFM_SELCHANGED)
	{
		SHGetPathFromIDList((LPCITEMIDLIST)lParam, szBuffer);
		SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szBuffer);
	}
	return 0;
}

BOOL BrowserFolder(HWND hWnd, TCHAR* szBuffer)
{
	BROWSEINFO stBrowserInfo;
	IMalloc * pMalloc = NULL;
	if (SHGetMalloc(&pMalloc) == E_FAIL)
	{
		OutputDebugString(TEXT("SHGetMallocʧ����"));
		return FALSE;
	}
	RtlZeroMemory(&stBrowserInfo, sizeof(stBrowserInfo));
	stBrowserInfo.hwndOwner = hWnd;
	BrowseFolderTmp = (DWORD)szBuffer;
	stBrowserInfo.lpfn = BrowserFolderCallBack;
	stBrowserInfo.lpszTitle = lpszTitle;
	stBrowserInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	LPITEMIDLIST pidlParent = SHBrowseForFolder(&stBrowserInfo);
	if (!pidlParent)
	{
		OutputDebugString(TEXT("SHBrowseForHandler error"));
		return FALSE;
	}
	HRESULT result = SHGetPathFromIDList(pidlParent, szBuffer);
	pMalloc->Free(pidlParent);
	pMalloc->Release();
	CoUninitialize();
	return TRUE;
}

BOOL FindFile(TCHAR* soucePath, PROCESSFILE_FUNC process_func)
{
	WIN32_FIND_DATA stFindFile;
	RtlZeroMemory(&stFindFile, sizeof(stFindFile));
	HANDLE hFindFile;

	TCHAR szPath[MAX_PATH];
	RtlZeroMemory(szPath, sizeof(szPath));

	TCHAR szSearch[MAX_PATH];
	RtlZeroMemory(szSearch, sizeof(szSearch));

	TCHAR szFindFile[MAX_PATH];
	RtlZeroMemory(szFindFile, sizeof(szFindFile));

	lstrcpy(szPath, soucePath);
	UINT length = lstrlen(szPath);
	TCHAR* szPath_01 = szPath;
	szPath_01 = szPath_01 + length;

	if ((*(szPath_01 - 1)) != '\\')
	{
		*szPath_01 = '\\';
	}

	lstrcpy(szSearch, szPath);
	lstrcat(szSearch, szFilter);

	hFindFile = FindFirstFile(szSearch, &stFindFile);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	do
	{
		lstrcpy(szFindFile, szPath);
		lstrcat(szFindFile, stFindFile.cFileName);
		if (stFindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((*stFindFile.cFileName) != '.')
			{
				dwFolderCount = dwFolderCount + 1;
				FindFile(szFindFile, process_func);
			}
		}
		else
		{
			//˵������һ���ļ�
			process_func(szFindFile);
		}
	} while (FindNextFile(hFindFile, &stFindFile));
	return TRUE;
}


CHAR* strrstr(CHAR* s, CONST CHAR *str, INT Len)
{
	CHAR* p;
	if (Len <= 0)
	{
		Len = strlen(s);
	}
	for (p = s + Len - 1; p >= s; p--)
	{
		if ((*p == *str) && (memcmp(p, str, strlen(str) == 0)))
		{
			return p;
		}
	}
	return NULL;
}

//����һ����ֹ��Χ�Ϳ�ʼ��ַ���Ӹߵ�ַ���͵�ַ����ɨ��
CHAR* strrstr2(CHAR* startP, CHAR* endP, CONST CHAR* szTargetStr)
{
	if (NULL == startP)
	{
		return NULL;
	}

	if (NULL == endP)
	{
		return NULL;
	}

	if (NULL == szTargetStr)
	{
		return NULL;
	}

	int nLen = strlen(szTargetStr);

	CHAR* successor = startP;


	while (successor >= endP)
	{
		if (*successor == *szTargetStr)
		{
			if (memcmp(successor, szTargetStr, nLen) == 0)
			{
				return successor;
			}
		}
		successor--;
	}

	return NULL;
}