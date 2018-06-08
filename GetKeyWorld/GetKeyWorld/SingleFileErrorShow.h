#ifndef SINGLE_FILE_ERROR_SHOW_H_
#define SINGLE_FILE_ERROR_SHOW_H_

#include "windows.h"
#include "HandleMenuOption.h"
#include "ArrayPod.h"
#include "StringPod.h"
#include "AnalysisLog.h"
#include "SingleFileManager.h"

class OpenFileList
{
public:
	HWND hWindowHandle;
	char* strFilePath;

	OpenFileList()
	{
		strFilePath = NULL;
	}

	OpenFileList(const OpenFileList& src)
	{
		hWindowHandle = src.hWindowHandle;
		strFilePath = NULL;
	}

	~OpenFileList()
	{
		if (NULL != strFilePath)
		{
			strFilePath = NULL;
		}
	}

};

typedef struct _SingleFileUIThreadInfo
{
	char* strBuff;
	HWND hParent;
	DWORD dwThreadID;
	OpenFileList* pStOpenFileList;
}SingleFileUIThreadInfo;

class ShowKeyInfoCell
{
public:
	char* fileStartPointer;
	int fileStartPos;

	ShowKeyInfoCell()
	{
		fileStartPointer = NULL;
		fileStartPos = 0;
	}

};

class ShowKeyInfo
{
public:
	ShowKeyInfo()
	{
		pArrayPod = NULL;
		nShowTime = 0;
	}

	int nShowTime;
	TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod;
};


typedef struct _STSingleFileThreadOp
{
	HWND hViewHWnd;
	DWORD dwThreadID;
	HANDLE hThread;
}STSingleFileThreadOp;

class AnalysisLogModule;
class AnalysisBehaviorLine;

#define ShowOutDefaultLength MAX_PATH * 10

class SingleViewContainer
{
public:
	AnalysisLogModule* m_AnalysisModule;
	AnalysisBehaviorLine* m_BehaviorModule;
	STSingleFileThreadOp m_ThreadOpInfo;
	TStringPod<char, ShowKeyInfo*> m_PodAlreadyInList;
	TStringPod<char, OpenFileList> strPodAllOpenList;
	OpenFileList* m_CurrentFileList;

	HWND hMainDlg;
	HWND hRightDlg;
	HWND hLeftDlg;

	MyListView* m_LeftListView;
	~SingleViewContainer();
	SingleViewContainer();
};

extern MyListView* g_pSingleErrorFileLeftView;

INT CALLBACK SingleDlgProc(HWND, UINT, WPARAM, LPARAM);

void Handle_DLG_INIT_INFO(HWND hWnd, OpenFileList* pOpenFile);

void Handle_RIGHT_DLG_INIT_INFO(HWND hWnd, OpenFileList* pOpenFile);

void ReleaseViewRelatedInfo(HWND);

DWORD WINAPI SingleFileAnalysisThreadOp (LPVOID lpParam);

void InitSingleFileMainDlg(HWND hWnd);

void InitSingleFileRightDlg(HWND hWnd);

LRESULT HandleDoubleClickRightDlg(HWND hWnd, LPNMITEMACTIVATE lpInfo);

//往右边的editbox里面丢数据，hWnd是
void AppendInfoMation(HWND hRichEdit, char* szData);

void ShowCertainIndexLog(HWND hWnd, int index = -1);

DWORD WINAPI SingleDlgThreadProc(LPVOID lpParam);

#endif