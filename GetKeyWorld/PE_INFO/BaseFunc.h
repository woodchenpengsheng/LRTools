#pragma once
#include <stdio.h>
CHAR* strrstr(CHAR* s, CONST CHAR* str, INT Len);
CHAR* strrstr2(CHAR* s, CONST CHAR* str, INT len);

/*
	日志相关
*/
#define OUT_PUT_BUFFER_LENGTH 256


class WriteLog
{
public:
	void operator()(const CHAR* szFormat, ...);
	static WriteLog& getInstance();
	~WriteLog();
	static void Destroy();
private:
	WriteLog();
	WriteLog(const WriteLog&);
	WriteLog& operator=(const WriteLog&);
	static WriteLog* m_WriteLog;
};


class CMyFileInterface
{
public:
	BOOL virtual Init(TCHAR* szFilePath) = 0;
	BOOL virtual Clear() = 0;
};

class CMyFileRead :  public CMyFileInterface
{
};

class CMyFileWrite: public CMyFileInterface
{
public:
	BOOL virtual Write(const char* buffer, int nlength) = 0;
};

//内存映射文件读
class CMyFileMapView : public CMyFileRead
{
private:
	HANDLE hSourceFile;
	HANDLE hSourceFileMap;
	LPVOID lpSourceMemory;

	DWORD uSourceSize;
public:
	CMyFileMapView();
	~CMyFileMapView();

	BOOL Init(TCHAR* szFilePath);
	BOOL Clear();
};