#include "stdafx.h"

#pragma region 日志相关
WriteLog* WriteLog::m_WriteLog = NULL;

CHAR m_buff[OUT_PUT_BUFFER_LENGTH];

void WriteLog::operator()(const CHAR* szFormat, ...)
{
	va_list pArgs;
	va_start(pArgs, szFormat);
	ZeroMemory(m_buff, OUT_PUT_BUFFER_LENGTH*sizeof(CHAR));
	wvsprintfA(m_buff, szFormat, pArgs);
	OutputDebugString(m_buff);
	va_end(pArgs);
}

WriteLog::WriteLog()
{
	int nBuffSize = sizeof(m_buff);
	ZeroMemory(m_buff, sizeof(m_buff));
}

WriteLog& WriteLog::getInstance()
{
	if (NULL == m_WriteLog)
	{
		m_WriteLog = new WriteLog();
	}
	return *m_WriteLog;
}

WriteLog::~WriteLog()
{
	this->Destroy();
}

void WriteLog::Destroy()
{
	if (m_WriteLog)
	{
		delete m_WriteLog;
		m_WriteLog = NULL;
	}
}

#pragma endregion



CHAR* strrstr(CHAR* s, CONST CHAR* str, INT Len)
{
	CHAR* p;
	if (Len <= 0)
	{
		Len = strlen(s);
	}
	for (p = s + Len - 1; p >= s; p--)
	{
		if ((*p == *str) && (memcmp(p, str, strlen(str)) == 0))
		{
			return p;
		}
	}
	return NULL;
}

CHAR* strrstr2(CHAR* s, CONST CHAR* str, INT len)
{
	//这里其实可以用想办法进行页边界的检测，达到边界就不继续，判断失败就可以了。
	CHAR* p = s;
	INT startIndex = 0;
	do 
	{
		if ((*p == *str) && (memcmp(p, str, strlen(str)) == 0))
		{
			return p;
		}
		startIndex++;
		p--;
	} while (startIndex <= len);
	return NULL;
}

#pragma region 文件相关
CMyFileMapView::CMyFileMapView()
{
	hSourceFile = INVALID_HANDLE_VALUE;
	hSourceFileMap = INVALID_HANDLE_VALUE;
	lpSourceMemory = NULL;
	uSourceSize = 0;
}


CMyFileMapView::~CMyFileMapView()
{
	this->Clear();
}

BOOL CMyFileMapView::Clear()
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

	return TRUE;

}


BOOL CMyFileMapView::Init(TCHAR* szFilePath)
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

	uSourceSize = GetFileSize(hSourceFile, NULL);

	if (uSourceSize <= 0)
	{
		return FALSE;
	}

	if (hSourceFile == INVALID_HANDLE_VALUE)
	{
		WriteLog::getInstance()("源文件打不开啊~, %d"), GetLastError();
		return FALSE;
	}

	hSourceFileMap = CreateFileMapping(hSourceFile, NULL,
		PAGE_READWRITE, 0, 1024 * 1024, NULL);

	if (hSourceFileMap == NULL)
	{
		WriteLog::getInstance()("源文件创建内存映射文件失败\n %d", GetLastError());
		return FALSE;
	}
	lpSourceMemory = MapViewOfFile(hSourceFileMap,
		FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (NULL == lpSourceMemory)
	{
		return FALSE;
	}

	return TRUE;
}
#pragma endregion
