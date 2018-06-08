#include "windows.h"
#include "HandleMenuOption.h"
#include "ArrayPod.h"
#include "StringPod.h"

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

typedef struct _SingleFileUIThreadInfo
{
	char* strBuff;
	HWND hParent;
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


class SingleViewContainer
{
public:
	TStringPod<char, ShowKeyInfo*> m_PodAlreadyInList;
	AnalysisLogModule* m_AnalysisModule;
	AnalysisBehaviorLine* m_BehaviorModule;
	STSingleFileThreadOp m_ThreadOpInfo;

	HWND hMainDlg;
	HWND hRightDlg;
	HWND hLeftDlg;

	~SingleViewContainer();
	SingleViewContainer();
};

extern MyListView* g_pSingleErrorFileLeftView;
extern SingleViewContainer* g_pSingleViewContainer;

INT CALLBACK SingleDlgProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI SingleDlgThreadProc(LPVOID lpParam);

void Handle_DLG_INIT_INFO(HWND hWnd, OpenFileList* pOpenFile);

void ReleaseViewRelatedInfo();

DWORD WINAPI SingleFileAnalysisThreadOp (LPVOID lpParam);

void InitSingleFileMainDlg(HWND hWnd);

void InitSingleFileRightDlg(HWND hWnd);