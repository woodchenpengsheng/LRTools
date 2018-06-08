// GetKeyWorld.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GetKeyWorld.h"
#include "HandleMenuOption.h"
#include "BasicUtil.h"
#include "commctrl.h"
#include "Richedit.h"
#include "ListViewRelated.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HWND hMainDlg;									// 主窗口句柄
HINSTANCE hMainInstance;						// 应用程序实例句柄

StartLogOp op;
HANDLE HandleStartLogOpThreadHandle = INVALID_HANDLE_VALUE;
MyListView* g_pMainListView = NULL;

char* strMainColumnName[] =
{
	"编号",
	"文件名",
	NULL,
};

//HMENU hMenu;									//菜单句柄 

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);

INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	hMainInstance = hInstance;
	InitCommonControls();
	HMODULE hRichEd = LoadLibrary(TEXT("RichEd20.dll"));

	//hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_GETKEYWORLD));

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DLGMAIN), NULL, DlgProc, NULL);
	FreeLibrary(hRichEd);
}

void InitMainDlg(HWND hWnd)
{
	hMainDlg = hWnd;
	HWND hModuleTable = GetDlgItem(hWnd, IDC_MODULETABLE);
	g_pMainListView = new  MyListView(strMainColumnName, hModuleTable);

	if (CoInitialize(NULL) != S_OK)
	{
		MessageBox(NULL, TEXT("Coinitialize失败"), TEXT("CoInitialize失败"), MB_OK);
		return;
	}

	//定义表格外观
	SendMessage(hModuleTable, LVM_SETEXTENDEDLISTVIEWSTYLE, \
		0, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ShowWindow(hModuleTable, SW_SHOW);
	g_pMainListView->ClearResultView();
}


INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	LPNMHDR lpNMHDR = NULL;
	switch (message)
	{
	case WM_INITDIALOG:
		InitMainDlg(hWnd);
		break;
	case WM_CLOSE:
		EndDialog(hWnd,NULL);
		break;
	case WM_NOTIFY:
		//listView双击事件响应
		lpNMHDR = (LPNMHDR)lParam;
		switch (lpNMHDR->code)
		{
		case NM_DBLCLK:
			if (lpNMHDR->idFrom == IDC_MODULETABLE)
			{
				HandleDoubleClickListView(hWnd,(LPNMITEMACTIVATE)lParam);
			}
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_INPUTKEY:
			if (INVALID_HANDLE_VALUE != HandleStartLogOpThreadHandle)
			{
				MessageBox(NULL, TEXT("请等待output完成"), TEXT("请等待output完成"), MB_OK);
				return TRUE;
			}
			HandleOutPutSaveLogFile(hWnd);
			break;
		case ID_OpenFile:

			if (INVALID_HANDLE_VALUE != HandleStartLogOpThreadHandle)
			{
				MessageBox(NULL, TEXT("请等待output完成"), TEXT("请等待output完成"), MB_OK);
				return TRUE;
			}

			HandleOpenFileOp(hWnd, message, wParam, lParam);
			//MyDebugOutPut(TEXT("ID_OpenFile"));
			break;
		case IDC_STARTLOG:

			if (INVALID_HANDLE_VALUE != HandleStartLogOpThreadHandle)
			{
				MessageBox(NULL, TEXT("请等待output完成"), TEXT("请等待output完成"), MB_OK);
				return TRUE;
			}
			op.hWnd = hWnd;
			op.keyNumber = IDC_CURRENTKEY;

			DWORD threadID;
			HandleStartLogOpThreadHandle = CreateThread(NULL, 0, HandleStartLogOpThreadProc, &op, 0, &threadID);
			break;
			//
			//MyDebugOutPut(TEXT("startLOG"));
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		delete g_pMainListView;
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
