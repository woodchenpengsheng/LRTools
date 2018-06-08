#ifndef _ANALYSIS_BEHAVIOR_LINE_H_
#define _ANALYSIS_BEHAVIOR_LINE_H_

#include "SingleFileErrorShow.h"

class OpLinePassData
{

public:
	CHAR* buffer;
	CHAR* filePointer;
	int nStartPos;
	int nFileSize;
	OpLinePassData()
	{
		buffer = NULL;
		filePointer = NULL;
		nStartPos = 0;
		nFileSize = 0;
	}
	OpLinePassData(const OpLinePassData& src)
	{
		this->buffer = src.buffer;
		this->filePointer = src.filePointer;
		this->nStartPos = src.nStartPos;
		this->nFileSize = src.nFileSize;
	}

};

class AnalysisBehaviorLine
{
public:
	virtual void OpLine(OpLinePassData buffer) = 0;
};

class AnalysisBehaviorLine_Func01 : public AnalysisBehaviorLine
{
public:
	virtual void OpLine(OpLinePassData buffer);
	TStringPod<char, int>* podAlreadyInList;
};

class AnalysisBehaviorLine_Func02 : public AnalysisBehaviorLine
{
public:
	virtual void OpLine(OpLinePassData buffer);
	TStringPod<char, ShowKeyInfo*>* m_SingleFileList;
};
#endif // !_ANALYSIS_BEHAVIOR_LINE_H_
