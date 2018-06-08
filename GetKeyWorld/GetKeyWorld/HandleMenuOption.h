#ifndef _HANDLE_MENU_OPTION_H_
#define _HANDLE_MENU_OPTION_H_

#include "windows.h"
#include "BasicUtil.h"
#include "Commctrl.h"
#include "StringPod.h"

class OpenFileList;

typedef struct _StartLogOp
{
	HWND hWnd;
	int keyNumber;
}StartLogOp;

DWORD WINAPI HandleStartLogOpThreadProc(LPVOID lpParam);

LRESULT HandleOpenFileOp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT HandleStartLogOp(HWND hWnd, int ItemId);
LRESULT HandleOutPutSaveLogFile(HWND hWnd);
LRESULT HandleDoubleClickListView(HWND hWnd,LPNMITEMACTIVATE lpInfo);

extern TStringPod<char, OpenFileList> strPodAllOpenList;

#endif