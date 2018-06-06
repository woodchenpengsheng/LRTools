// PE_INFO.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PE_INFO.h"
#include "PE.h"
#include "commdlg.h"
#include "Richedit.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
HWND hRichEdit;
//我定义的全局变量
CPE gPE;
TCHAR szExtPe[] = _T("PE File\0*.exe;*.dll;*.scr;*.fon;*.drv\0All Files(*.*)\0*.*\0\0");

INT_PTR CALLBACK	MainDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK OpenPEFile(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID AppendInfoMation(TCHAR*);
VOID InitMainDlg(HWND);



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	hInst = hInstance;
	HMODULE hRichEd =  LoadLibrary(TEXT("RICHED20.DLL"));
	DialogBoxParam(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, MainDlgProc, NULL);
	FreeLibrary(hRichEd);
}

INT_PTR CALLBACK MainDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hWnd, NULL);
		break;
	case WM_INITDIALOG:
		InitMainDlg(hWnd);
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
		case ID_32771:
			if (TRUE == OpenPEFile(hWnd, message, wParam, lParam))
			{
				gPE.OutPutImportInfo();
			}
			break;
		case ID_32772:

			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}
	return 0;
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

BOOL CALLBACK OpenPEFile(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szTmpFileName[MAX_PATH];
	ZeroMemory(szTmpFileName, MAX_PATH*sizeof(TCHAR));

	OPENFILENAME stOpenFile;
	ZeroMemory(&stOpenFile, sizeof(stOpenFile));
	stOpenFile.lStructSize = sizeof(stOpenFile);
	stOpenFile.hwndOwner = hWnd;
	stOpenFile.lpstrFilter = szExtPe;
	stOpenFile.lpstrFile = szTmpFileName;
	stOpenFile.nMaxFile = MAX_PATH;
	stOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (FALSE == GetOpenFileName(&stOpenFile))
	{
		MessageBox(NULL, TEXT("打开文件失败"), TEXT("提示"), MB_OK);
		return FALSE;
	}
	if (FALSE == gPE.InitPE(szTmpFileName))
	{
		return FALSE;
	}
	return TRUE;
}


VOID AppendInfoMation(TCHAR* szData)
{
	CHARRANGE stCharRange;
	int nLength = GetWindowTextLength(hRichEdit);
	stCharRange.cpMax = nLength;
	stCharRange.cpMin = nLength;
	SendMessage(hRichEdit, EM_EXSETSEL, 0, (LPARAM)&stCharRange);
	SendMessage(hRichEdit, EM_REPLACESEL, FALSE, (LPARAM)szData);
}

VOID InitMainDlg(HWND hMainDlg)
{
	hRichEdit = GetDlgItem(hMainDlg, IDC_EDIT1);
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(hMainDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hRichEdit, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
	CHARFORMAT stFormat;
	ZeroMemory(&stFormat, sizeof(CHARFORMAT));
	stFormat.cbSize = sizeof(stFormat);
	stFormat.yHeight = 9 * 20;
	stFormat.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD;
	lstrcpy(stFormat.szFaceName, TEXT("宋体"));
	SendMessage(hRichEdit, EM_SETCHARFORMAT, 0, (LPARAM)&stFormat);
	SendMessage(hRichEdit, EM_EXLIMITTEXT, 0, -1);
}