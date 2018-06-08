#include "stdafx.h"
#include "AnalysisBehaviorLine.h"

void AnalysisBehaviorLine_Func01::OpLine(OpLinePassData sTData)
{
	CHAR* buff = st.buffer;
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

void AnalysisBehaviorLine_Func02::OpLine(OpLinePassData sTData)
{
	if (NULL != m_SingleFileList)
	{
		TStringPod<char, ShowKeyInfo*>::iterator iter = m_SingleFileList->Find(sTData.buffer);
		if (iter == m_SingleFileList->End())
		{
			ShowKeyInfo* pShowInfo = new ShowKeyInfo();
			pShowInfo->pArrayPod = new TArrayPod<ShowKeyInfoCell*, 1>();
			pShowInfo->nShowTime = 1;

			ShowKeyInfoCell* pShowCell = new ShowKeyInfoCell();

			TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowInfo->pArrayPod;

			pArrayPod->push_back(pShowCell);

			pShowCell->fileStartPointer = sTData.filePointer;
			pShowCell->fileStartPos = sTData.nStartPos;

			//MyDebugOutPut(buff);
			//这个是深拷贝
			m_SingleFileList->Add(sTData.buffer, pShowInfo);
		}
		else
		{
			ShowKeyInfo* pShowInfo = iter.GetData();
			pShowInfo->nShowTime = pShowInfo->nShowTime + 1;

			TArrayPod<ShowKeyInfoCell*, 1>* pArrayPod = pShowInfo->pArrayPod;
			ShowKeyInfoCell* pShowCell = new ShowKeyInfoCell();
			pArrayPod->push_back(pShowCell);

			pShowCell->fileStartPointer = sTData.filePointer;
			pShowCell->fileStartPos = sTData.nStartPos;
		}
	}
	else
	{
		MyDebugOutPut(sTData.buffer);
	}
}