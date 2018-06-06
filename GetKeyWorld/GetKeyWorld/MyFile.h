#ifndef _MY_FILE_H_
#define _MY_FILE_H_

//用内存映射方式创建
class MyFile
{
private:
	HANDLE hSourceFile;
	HANDLE hSourceFileMap;
	LPVOID lpSourceMemory;
	DWORD uSourceSize;

public:
	MyFile();
	~MyFile();

	BOOL Init(TCHAR* szFilePath);
	BOOL Clear();
	BOOL Write(const char* buffer, int nlength);
};


#endif