#ifndef _ANALYSIS_BEHAVIOR_LINE_H_
#define _ANALYSIS_BEHAVIOR_LINE_H_

class AnalysisBehaviorLine
{
public:
	virtual void OpLine(CHAR* buffer) = 0;
};

class AnalysisBehaviorLine_Func01 : public AnalysisBehaviorLine
{
public:
	virtual void OpLine(CHAR* buffer);
	TStringPod<char, int>* podAlreadyInList;
};

class AnalysisBehaviorLine_Func02 : public AnalysisBehaviorLine
{
public:
	virtual void OpLine(CHAR* buffer);
};
#endif // !_ANALYSIS_BEHAVIOR_LINE_H_
