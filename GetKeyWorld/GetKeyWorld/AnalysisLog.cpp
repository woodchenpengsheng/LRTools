#include "stdafx.h"



AnalysisLogModule::AnalysisLogModule()
{
	hSourceFile = INVALID_HANDLE_VALUE;
	hSourceFileMap = INVALID_HANDLE_VALUE;
	lpSourceMemory = NULL;
	OutPut = NULL;
	uSourceSize = 0;
}


BOOL AnalysisLogModule::Init(TCHAR* szFilePath, CHAR* szWantKey)
{
	this->szWantKey = szWantKey;
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

BOOL AnalysisLogModule::Clear()
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

	if (NULL != OutPut)
	{
		OutPut->Clear();
	}

	OutPut = NULL;
	uSourceSize = 0;

	return TRUE;
}

void AnalysisLogModule::SetAlreadyInList(TStringPod<char, int>* podList)
{
	podAlreadyInList = podList;
}

void AnalysisLogModule::AnalysisTest()
{
	char testData[] = "[2018-05-28 20:30:31] [SceneBaseModule::LoadSceneInfo:1029][ERROR]:scene id error.\n\
		[2018 - 05 - 28 20:30 : 32] store_1_0 connector_id 6 address 127.0.0.1:59013 accept, total 4\
		[2018 - 05 - 28 20:30 : 32] member_1_0 connector_id 0 address 127.0.0.1 : 58963 comm_id 4 connected, total \
		[2018 - 05 - 28 20:30 : 32] cache_1 connector_id 1 address 127.0.0.1 : 59015 accept, total 2\
		[2018-05-28 20:31:03] [TeamModule::OnCustomGetTeamListInfo:2710][ERROR]:OnCustomGetTeamListInfo fail, get team main rec fail!\n\
		[2018 - 05 - 28 20:31 : 03][Domain_Channel::LeaveChannelByType:376][INFO]:Player already in channel.\
		[2018 - 05 - 28 20:31 : 03][Domain_Channel::LeaveChannelByType:377][INFO] : EE074C0993BE431E8DDEA8868F58CDF8\
		[2018 - 05 - 28 20:31 : 03][Domain_Channel::LeaveChannelByType:378][INFO] : 3\
		";
	int length = strlen(testData);
	lpSourceMemory = testData;

	char* startP = (char*)lpSourceMemory;
	char* endP = (char*)lpSourceMemory + length - 1;
	if (NULL == startP)
	{
		return;
	}

	const char* wantKey = "[ERROR]";
	int wantKeyLength = strlen(wantKey);

	while (startP < endP)
	{
		char* targetResult = strstr(startP, wantKey);
		if (NULL == targetResult)
		{
			return;
		}

		char* nextStart = targetResult + wantKeyLength;
		char* nextLine = strstr(nextStart, "\n");

		if (NULL == nextLine)
		{
			return;
		}

		int infoLength = nextLine - nextStart;
		char* buff = new char[infoLength + 2];
		ZeroMemory(buff, (infoLength + 2) * sizeof(char));

		strncpy(buff, nextStart, infoLength);
		buff[infoLength] = '\n';

		if (NULL != podAlreadyInList)
		{
			if (podAlreadyInList->Find(buff) == podAlreadyInList->End())
			{
				MyDebugOutPut(buff);
				//这个是深拷贝
				podAlreadyInList->Add(buff, true);
			}
		}

		delete buff;
		
		startP = nextLine + 1;
	}
	
}

BOOL AnalysisLogModule::AnalysisStart()
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

		//char* thisLineStartFlag = strrstr2(targetResult, (char*)lpSourceMemory, "\n");
		char* thisLineStartFlag = strrstr2(targetResult - 1, (char*)lpSourceMemory, "[");

		if (NULL == thisLineStartFlag)
		{
			MessageBox(NULL, TEXT("文件格式异常"), TEXT("文件格式异常！！！"), MB_OK);
			return FALSE;
		}

		nextStart = thisLineStartFlag;

		//char* nextStart = targetResult + wantKeyLength;
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

		//[2018 - 05 - 28 13:49 : 35][SkillModule::GetMaxSkillRange:203][ERROR]:0，进行比较的时候需要将事件给剔除
		

		if (NULL != podAlreadyInList)
		{
			if (podAlreadyInList->Find(buff) == podAlreadyInList->End())
			{
				if (this->OutPut)
				{
					//由于需要增加次数，这里不在直接写了
					//this->OutPut->Write(buff, infoLength);
				}
				else
				{
					MyDebugOutPut(buff);
				}
				//这个是深拷贝
				podAlreadyInList->Add(buff, 1);
			}
			else
			{
				int currNumber = 0;
				podAlreadyInList->GetData(buff, currNumber);
				//if (strstr(buff, "[EquipmentModule::CanEquip:215][ERROR]"))
				//{
				//	int a = 0; 
				//	a++;
				//}
				currNumber = currNumber + 1;
				podAlreadyInList->Set(buff, currNumber);
			}
		}

		delete buff;

		startP = nextLine + 1;
	}
	return TRUE;
}


void AnalysisLogModule::SetMyFile(MyFile* outPut)
{
	this->OutPut = outPut;
}