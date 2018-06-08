#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_
#include "StringPod.h"
#include "MyFile.h"
#include "AnalysisBehaviorLine.h"

class AnalysisLogBasic
{
protected:
	char szWantKey[MAX_PATH];
	AnalysisBehaviorLine* m_LineOp;
	HANDLE hSourceFile;
	HANDLE hSourceFileMap;
	LPVOID lpSourceMemory;
	DWORD uSourceSize;

public:
	virtual ~AnalysisLogBasic();
	AnalysisLogBasic();
	virtual BOOL Init(TCHAR* szFilePath);
	virtual BOOL Clear();
	virtual BOOL AnalysisStart();
	void SetLineOp(AnalysisBehaviorLine* op);
	AnalysisBehaviorLine* GetLineOp();
	virtual void SetWantKey(char* strKey);

	DWORD GetSize();
	LPVOID GetStartSourceMemory();
};


class AnalysisLogModule : public AnalysisLogBasic
{
public:
	~AnalysisLogModule();
	AnalysisLogModule();
	BOOL Clear();
	void AnalysisTest();
};

//class AnalysisSingleFileModule : public AnalysisLogBasic
//{
//public:
//	~AnalysisSingleFileModule();
//	AnalysisSingleFileModule();
//	virtual BOOL Clear();
//};
#endif