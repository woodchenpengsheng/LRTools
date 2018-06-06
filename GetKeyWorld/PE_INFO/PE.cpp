#include "stdafx.h"
#include "PE.h"

extern VOID AppendInfoMation(TCHAR* szData);

CPE::CPE()
{
	InitValue();
}


CPE::~CPE()
{
}

VOID CPE::InitValue()
{
	m_hFile = NULL;
	m_dwFileSize = 0;
	m_dwImageSize = 0;
	m_pDosHeader = NULL;
	m_pNtHeader = NULL;

	m_dwMemAlign = 0;
	m_dwSizeOfHeader = 0;
	m_dwSectionNum = 0;

	m_dwFileAlign = 0;
	m_dwImageBase = 0;
	m_dwPEOEP = 0;
	m_dwCodeBase = 0;
	m_dwCodeSize = 0;

	m_PERelocDir = { 0 };
	m_PEImportDir = { 0 };

	m_IATSectionBase = 0;
	m_IATSectionSize = 0;

	m_pSecHeader = NULL;
	if (m_pFileImageBuffer)
	{
		delete[] m_pFileImageBuffer;
		m_pFileImageBuffer = NULL;
	}

	OriginalCloseFileHandle();
}

BOOL CPE::InitPE(TCHAR* strFilePath)
{
	m_hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		WriteLog::getInstance()("CreateFile失效，输入的路径名字为：%s\n", strFilePath);
		return FALSE;
	}
	m_dwFileSize = GetFileSize(m_hFile, NULL);
	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (NULL == m_hFileMapping)
	{
		WriteLog::getInstance()("创建内存映射文件失败！\n");
		return FALSE;
	}
	m_pFileMappingView = MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
	//判断是否为PE文件
	if (!IsPE())
	{
		return FALSE;
	}

	//下面这一部分仅仅是为了将PE文件->放到内存中，相当于成为内存映像
	m_dwImageSize = m_pNtHeader->OptionalHeader.SizeOfImage;
	m_dwMemAlign = m_pNtHeader->OptionalHeader.SectionAlignment;
	m_dwSizeOfHeader = m_pNtHeader->OptionalHeader.SizeOfHeaders;
	m_dwSectionNum = m_pNtHeader->FileHeader.NumberOfSections;

	if (m_dwImageSize % m_dwMemAlign)
	{
		m_dwImageSize = (m_dwImageSize / m_dwMemAlign + 1) * m_dwMemAlign;
	}
	LPBYTE pFileBuff_New = new BYTE[m_dwImageSize];
	memset(pFileBuff_New, 0, m_dwImageSize);
	//拷贝文件头
	memcpy_s(pFileBuff_New, m_dwSizeOfHeader, m_pFileMappingView, m_dwSizeOfHeader); //DOS+NT+节表在映射之后是完全一样的
	//节表里面的内容需要进行对齐
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_dwSectionNum; i++, pSectionHeader++)
	{
		memcpy_s(pFileBuff_New + pSectionHeader->VirtualAddress,
			pSectionHeader->SizeOfRawData,
			(PBYTE)m_pDosHeader + pSectionHeader->PointerToRawData,
			pSectionHeader->SizeOfRawData);
	}
	m_pFileImageBuffer = pFileBuff_New;
	pFileBuff_New = NULL;
	GetPEInfo();
	OriginalCloseFileHandle();
	return TRUE;
}

BOOL CPE::IsPE()
{
	if (m_pFileMappingView <= 0)
	{
		return FALSE;
	}
	m_pDosHeader = (PIMAGE_DOS_HEADER)m_pFileMappingView;
	if (m_pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		WriteLog::getInstance()("e_magic不是DOS");
		InitValue();
		return FALSE;
	}
	m_pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_pDosHeader + m_pDosHeader->e_lfanew);
	if (m_pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		WriteLog::getInstance()("Signature不是PE");
		InitValue();
		return FALSE;
	}

	return TRUE;
}


void CPE::GetPEInfo()
{
	WriteLog::getInstance()("%s", "wood");
	m_pDosHeader = (PIMAGE_DOS_HEADER)m_pFileImageBuffer;
	m_pNtHeader = (PIMAGE_NT_HEADERS)(m_pFileImageBuffer + m_pDosHeader->e_lfanew);

	m_dwFileAlign = m_pNtHeader->OptionalHeader.FileAlignment;
	m_dwMemAlign = m_pNtHeader->OptionalHeader.SectionAlignment;
	m_dwImageBase = m_pNtHeader->OptionalHeader.ImageBase;
	m_dwPEOEP = m_pNtHeader->OptionalHeader.AddressOfEntryPoint;
	m_dwCodeBase = m_pNtHeader->OptionalHeader.BaseOfCode;
	m_dwCodeSize = m_pNtHeader->OptionalHeader.SizeOfCode;
	m_dwSizeOfHeader = m_pNtHeader->OptionalHeader.SizeOfHeaders;
	m_dwSectionNum = m_pNtHeader->FileHeader.NumberOfSections;

	m_pSecHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	m_pNtHeader->OptionalHeader.SizeOfImage = m_dwImageSize;



	//保存重定位目录信息
	m_PERelocDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);

	//保存導入表
	m_PEImportDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

	//获取IAT所在的区段的起始位置和大小
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_dwSectionNum; i++, pSectionHeader++)
	{
		if (m_PEImportDir.VirtualAddress >= pSectionHeader->VirtualAddress&&
			m_PEImportDir.VirtualAddress < pSectionHeader[1].VirtualAddress)
		{
			//保存该区段的起始地址和大小
			m_IATSectionBase = pSectionHeader->VirtualAddress;
			m_IATSectionSize = pSectionHeader[1].VirtualAddress - pSectionHeader->VirtualAddress;
			break;
		}
	}
}

void CPE::OriginalCloseFileHandle()
{
	if (NULL != m_pFileMappingView)
	{
		UnmapViewOfFile(m_pFileMappingView);
		m_pFileMappingView = NULL;
	}
	if (NULL != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if (NULL != m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}

void CPE::OutPutImportInfo()
{
	if (0 == m_PEImportDir.VirtualAddress)
	{
		AppendInfoMation(_T("没有发现导入表！"));
		return;
	}
	//TCHAR SectionNameBuffer[MAX_PATH];
	//ZeroMemory(SectionNameBuffer, MAX_PATH*sizeof(TCHAR));
	TCHAR tempBuffer[1024];
	ZeroMemory(tempBuffer, 1024 * sizeof(TCHAR));

	PIMAGE_IMPORT_DESCRIPTOR pPEImport = (PIMAGE_IMPORT_DESCRIPTOR)(m_pFileImageBuffer + m_PEImportDir.VirtualAddress);
	PBYTE SectionName = GetRVASectionName((DWORD)m_PEImportDir.VirtualAddress);
	//MultiByteToWideChar(CP_ACP, 0, (CHAR*)SectionName, -1, tempBuffer, 0);
	wsprintf(tempBuffer, "---------------------------------------------------------------------------------------"
		"导入表所在的节：%s \r\n\r\n", SectionName);
	AppendInfoMation(tempBuffer);

	while (pPEImport->Name||pPEImport->OriginalFirstThunk
		||pPEImport->FirstThunk||pPEImport->Name||pPEImport->ForwarderChain)
	{
		ZeroMemory(tempBuffer, sizeof(TCHAR)* 1024);
		wsprintf(tempBuffer, "导入库：%s\r\n\r\n"
			"-------------------NEW--------------------\r\n"
			"\tOriginalFirstThunk	%08x\r\n\r\n"
			"\tTimeDataStamp %08x\r\n\r\n"
			"\tForwarderChain %08x\r\n\r\n"
			"\tFirstThunk %08x\r\n\r\n"
			"------------------------------------------\r\n",
			m_pFileImageBuffer + pPEImport->Name,pPEImport->OriginalFirstThunk,pPEImport->TimeDateStamp,
			pPEImport->ForwarderChain,pPEImport->FirstThunk);
		AppendInfoMation(tempBuffer);
		PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(pPEImport->OriginalFirstThunk + m_pFileImageBuffer);
		while (pINT->u1.AddressOfData)
		{
			ZeroMemory(tempBuffer, sizeof(TCHAR));

			if (IMAGE_SNAP_BY_ORDINAL(pINT->u1.Ordinal))
			{
				//说明是序号导出方式
				DWORD dwFunOrdinal = pINT->u1.Ordinal&IMAGE_ORDINAL_FLAG;
				wsprintf(tempBuffer, "%08u(这是一个没有名字的导入函数):%lu \r\n", dwFunOrdinal);
			}
			else
			{
				DWORD dwFunNameRVA = pINT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFuncName = PIMAGE_IMPORT_BY_NAME(dwFunNameRVA + m_pFileImageBuffer);
				wsprintf(tempBuffer, "%08u  %s\r\n", pstcFuncName->Hint, pstcFuncName->Name);
			}
			AppendInfoMation(tempBuffer);
			pINT++;
		}
		AppendInfoMation(TEXT("\r\n\r\n"));
		pPEImport++;
	}
}

PBYTE CPE::GetRVASectionName(DWORD dwAddr)
{
	DWORD totalSectionNum = m_dwSectionNum;
	if (totalSectionNum <= 0)
	{
		return NULL;
	}
	PIMAGE_SECTION_HEADER pFirstSectionHeader = m_pSecHeader;
	for (DWORD i = 0; i < totalSectionNum; i++, pFirstSectionHeader++)
	{
		if (pFirstSectionHeader->VirtualAddress <= dwAddr && dwAddr < pFirstSectionHeader[1].VirtualAddress)
		{
			return pFirstSectionHeader->Name;
		}
	}
	return NULL;
}