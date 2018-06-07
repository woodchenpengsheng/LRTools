#include "windows.h"
#include "HandleMenuOption.h"

typedef struct _SingleFileUIThreadInfo
{
	char* strBuff;
	HWND hParent;
}SingleFileUIThreadInfo;

extern MyListView* g_pSingleErrorFileLeftView;

INT CALLBACK SingleDlgProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI SingleDlgThreadProc(LPVOID lpParam);

void Handle_DLG_INIT_INFO(HWND hWnd, OpenFileList* pOpenFile);