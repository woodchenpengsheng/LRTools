#include "stdafx.h"
#include "AnalysisBehaviorLine.h"

void AnalysisBehaviorLine_Func01::OpLine(CHAR* buff)
{
	if (NULL != podAlreadyInList)
	{
		if (podAlreadyInList->Find(buff) == podAlreadyInList->End())
		{
			//MyDebugOutPut(buff);
			//这个是深拷贝
			podAlreadyInList->Add(buff, 1);
		}
		else
		{
			int currNumber = 0;
			podAlreadyInList->GetData(buff, currNumber);
			currNumber = currNumber + 1;
			podAlreadyInList->Set(buff, currNumber);
		}
	}
	else
	{
		MyDebugOutPut(buff);
	}
}

void AnalysisBehaviorLine_Func02::OpLine(CHAR* buff)
{

}