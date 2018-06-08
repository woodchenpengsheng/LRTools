#include "stdafx.h"

#pragma region AnalysisBasic模块

AnalysisLogBasic::AnalysisLogBasic()
{
	hSourceFile = INVALID_HANDLE_VALUE;
	hSourceFileMap = INVALID_HANDLE_VALUE;
	lpSourceMemory = NULL;
	uSourceSize = 0;
	m_LineOp = NULL;
	ZeroMemory(szWantKey, MAX_PATH);
}

void AnalysisLogBasic::SetWantKey(char* strKey)
{
	if (NULL == strKey)
		return;
	
	int nLength = strlen(strKey);
	if (nLength >= MAX_PATH - 1)
	{
		return;
	}
	memcpy(szWantKey, strKey, nLength);
	szWantKey[nLength] = '\0';
}

BOOL AnalysisLogBasic::Init(TCHAR* szFilePath)
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
	uSourceSize = GetFileSize(hSourceFile, NULL);

	if (hSourceFile == INVALID_HANDLE_VALUE)
	{
		MyDebugOutPut(TEXT("源文件打不开啊~"));
		return FALSE;
	}

	hSourceFileMap = CreateFileMapping(hSourceFile, NULL,
		PAGE_READWRITE, 0, 0, NULL);

	if (hSourceFileMap == INVALID_HANDLE_VALUE)
	{
		MyDebugOutPut(TEXT("源文件创建内存映射文件失败\n"));
		return FALSE;
	}
	lpSourceMemory = MapViewOfFile(hSourceFileMap,
		FILE_MAP_ALL_ACCESS, 0, 0, 0);
	return TRUE;
}

BOOL AnalysisLogBasic::Clear()
{
	if (NULL != lpSourceMemory)
	{
		UnmapViewOfFile(lpSourceMemory);
		lpSourceMemory = NULL;
	}

	if (hSourceFileMap != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSourceFileMap);
		hSourceFileMap = INVALID_HANDLE_VALUE;
	}

	if (hSourceFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSourceFile);
		hSourceFile = INVALID_HANDLE_VALUE;
	}

	uSourceSize = 0;
	return TRUE;
}

AnalysisLogBasic::~AnalysisLogBasic()
{
	AnalysisLogBasic::Clear();
}

BOOL AnalysisLogBasic::AnalysisStart()
{
	if (NULL == lpSourceMemory)
	{
		return FALSE;
	}

	char* startP = (char*)lpSourceMemory;
	char* endP = (char*)lpSourceMemory + uSourceSize - 1;

	if (NULL == startP)
	{
		return FALSE;
	}

	int wantKeyLength = strlen(szWantKey);

	while (startP < endP)
	{
		char* targetResult = strstr(startP, szWantKey);

		if (NULL == targetResult)
		{
			return FALSE;
		}


		char* nextStart = NULL;

		char* thisLineStartFlag = strrstr2(targetResult - 1, (char*)lpSourceMemory, "[");

		if (NULL == thisLineStartFlag)
		{
			MessageBox(NULL, TEXT("文件格式异常"), TEXT("文件格式异常！！！"), MB_OK);
			return FALSE;
		}

		nextStart = thisLineStartFlag;

		char* nextLine = strstr(nextStart, "\n");

		if (NULL == nextLine)
		{
			return FALSE;
		}

		int infoLength = nextLine - nextStart;
		char* buff = new char[infoLength + 2];
		ZeroMemory(buff, (infoLength + 2) * sizeof(char));

		strncpy(buff, nextStart, infoLength);
		buff[infoLength] = '\n';

		if (m_LineOp)
		{
			OpLinePassData data;
			data.buffer = buff;
			data.filePointer = thisLineStartFlag;
			data.nFileSize = uSourceSize;
			data.nStartPos = thisLineStartFlag - (char*)lpSourceMemory;
			m_LineOp->OpLine(data);
		}

		delete buff;

		startP = nextLine + 1;
	}
	return TRUE;
}

void AnalysisLogBasic::SetLineOp(AnalysisBehaviorLine* op)
{
	this->m_LineOp = op;
}

AnalysisBehaviorLine* AnalysisLogBasic::GetLineOp()
{
	return this->m_LineOp;
}

#pragma endregion

#pragma region 主界面输出模块

AnalysisLogModule::~AnalysisLogModule()
{
	AnalysisLogModule::Clear();
}

AnalysisLogModule::AnalysisLogModule()
{
}


BOOL AnalysisLogModule::Clear()
{
	return AnalysisLogBasic::Clear();
}

#pragma endregion