#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_
#include "StringPod.h"
#include "MyFile.h"

class AnalysisLogModule
{
private:
	HANDLE hSourceFile;
	HANDLE hSourceFileMap;
	LPVOID lpSourceMemory;
	DWORD uSourceSize;

	TStringPod<char, int>* podAlreadyInList;

	//const char* wantKey = "[ERROR]";
	//int wantKeyLength = strlen(wantKey);
	char* szWantKey;

public:
	MyFile* OutPut;
	AnalysisLogModule();
	BOOL Init(TCHAR* szFilePath, CHAR* szWantKey);
	BOOL Clear();
	BOOL AnalysisStart();
	void AnalysisTest();
	void SetAlreadyInList(TStringPod<char, int>*);
	void SetMyFile(MyFile* outPut);
};
#endif