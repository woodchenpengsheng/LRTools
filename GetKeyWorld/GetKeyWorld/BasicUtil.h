#ifndef _BASIC_UTIL_H_
#define _BASIC_UTIL_H_

#include "windows.h"

//�Զ���ĵ��������
void MyDebugOutPut(const TCHAR* szFormat, ...);

//����һ��Ŀ¼����Ŀ¼�°��ո���չ����β�ĵ��ļ������յ�����ص�������
typedef BOOL(PROCESSFILE_FUNC)(TCHAR*);

BOOL BrowserFolder(HWND hWnd, TCHAR* szBuffer);

//�ݹ�Ƕ�ײ���ָ����չ����β���ļ���PROCESSFILE_FUNCΪ������
BOOL FindFile(TCHAR* soucePath, PROCESSFILE_FUNC process_func);

CHAR* strrstr(CHAR* s, CONST CHAR *str, INT uLen = -1);

//����һ����ֹ��Χ�Ϳ�ʼ��ַ���Ӹߵ�ַ���͵�ַ����ɨ��
CHAR* strrstr2(CHAR* startP, CHAR* endP, CONST CHAR* szTargetStr);

extern DWORD BrowseFolderTmp;
extern TCHAR lpszTitle[];
extern TCHAR szSelect[];
extern TCHAR szNoSelect[];
extern TCHAR szFilter[];

extern UINT dwFolderCount; //���ļ�����Ŀ
extern UINT dwFileCount; //�ܹ��ж��ٸ��ļ�
extern ULONG dwFileTotalSize; //�ļ��Ĵ�С

extern UINT dwFileCountFirst;
#define  OUT_PUT_BUFFET_LENGTH 256
extern TCHAR outPutBuffer[];
extern TCHAR newFileExtension[];


#endif