#ifndef _BASIC_UTIL_H_
#define _BASIC_UTIL_H_

#include "windows.h"

//自定义的调试输出流
void MyDebugOutPut(const TCHAR* szFormat, ...);

//给定一个目录，该目录下按照该拓展名结尾的的文件都会收到这个回调函数。
typedef BOOL(PROCESSFILE_FUNC)(TCHAR*);

BOOL BrowserFolder(HWND hWnd, TCHAR* szBuffer);

//递归嵌套查找指定拓展名结尾的文件，PROCESSFILE_FUNC为处理函数
BOOL FindFile(TCHAR* soucePath, PROCESSFILE_FUNC process_func);

CHAR* strrstr(CHAR* s, CONST CHAR *str, INT uLen = -1);

//给定一个终止范围和开始地址，从高地址往低地址进行扫描
CHAR* strrstr2(CHAR* startP, CHAR* endP, CONST CHAR* szTargetStr);

extern DWORD BrowseFolderTmp;
extern TCHAR lpszTitle[];
extern TCHAR szSelect[];
extern TCHAR szNoSelect[];
extern TCHAR szFilter[];

extern UINT dwFolderCount; //总文件夹数目
extern UINT dwFileCount; //总共有多少个文件
extern ULONG dwFileTotalSize; //文件的大小

extern UINT dwFileCountFirst;
#define  OUT_PUT_BUFFET_LENGTH 256
extern TCHAR outPutBuffer[];
extern TCHAR newFileExtension[];


#endif