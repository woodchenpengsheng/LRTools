#include "stdafx.h"

MyFile::MyFile()
{
	hSourceFile = INVALID_HANDLE_VALUE;
	hSourceFileMap = INVALID_HANDLE_VALUE;
	lpSourceMemory = NULL;

	uSourceSize = 0;
}


MyFile::~MyFile()
{
	this->Clear();
}

BOOL MyFile::Clear()
{
	//if (NULL != lpSourceMemory)
	//{
	//	UnmapViewOfFile(lpSourceMemory);
	//	lpSourceMemory = NULL;
	//}

	//if (hSourceFileMap != INVALID_HANDLE_VALUE)
	//{
	//	CloseHandle(hSourceFileMap);
	//	hSourceFileMap = INVALID_HANDLE_VALUE;
	//}

	if (hSourceFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSourceFile);
		hSourceFile = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}


BOOL MyFile::Init(TCHAR* szFilePath)
{
	if (!this->Clear())
	{
		return FALSE;
	}

	if (NULL == szFilePath)
	{
		return FALSE;
	}

	hSourceFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (hSourceFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//uSourceSize = GetFileSize(hSourceFile, NULL);

	//if (uSourceSize <= 0)
	//{

	//}

	//if (hSourceFile == INVALID_HANDLE_VALUE)
	//{
	//	MyDebugOutPut(TEXT("源文件打不开啊~, %d"), GetLastError());
	//	return FALSE;
	//}

	//hSourceFileMap = CreateFileMapping(hSourceFile, NULL,
	//	PAGE_READWRITE, 0, 1024*1024, NULL);

	//if (hSourceFileMap == NULL)
	//{
	//	MyDebugOutPut(TEXT("源文件创建内存映射文件失败\n %d"), GetLastError());
	//	return FALSE;
	//}
	//lpSourceMemory = MapViewOfFile(hSourceFileMap,
	//	FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//if (NULL == lpSourceMemory)
	//{
	//	return FALSE;
	//}
	return TRUE;
}


BOOL MyFile::Write(const char* buffer, int nlength)
{
	//if (NULL == lpSourceMemory)
	//{
	//	return FALSE;
	//}

	DWORD dwWritenSize = 0;
	if (hSourceFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bRet = WriteFile(hSourceFile, buffer, nlength, &dwWritenSize, NULL);
	if (!bRet)
	{
		MessageBox(NULL, TEXT("写入数据失败！！"), TEXT("WARNING"), MB_OK);
		return FALSE;
	}

	FlushFileBuffers(hSourceFile);

	//strncpy((char*)lpSourceMemory, buffer, nlength);

	//lpSourceMemory = (char*)lpSourceMemory + nlength;
	return TRUE;
}