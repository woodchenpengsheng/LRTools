#pragma once
class CPE
{
public:
	CPE();
	~CPE();
public:
	HANDLE m_hFile;	//PE文件句柄
	PIMAGE_DOS_HEADER m_pDosHeader; //Dos头
	PIMAGE_NT_HEADERS m_pNtHeader; //Nt头
	DWORD m_dwFileSize; //PE文件大小
	DWORD m_dwImageSize; //PE文件内存映像大小
	DWORD m_dwMemAlign; //PE内存对齐尺寸
	DWORD m_dwSizeOfHeader; //除了节内容的整个内存映像的大小
	DWORD m_dwSectionNum; //节的数量

	DWORD m_dwFileAlign;//文件对齐
	DWORD m_dwImageBase;//建议装在基地址
	DWORD m_dwPEOEP; //入口
	DWORD m_dwCodeBase; //代码起始
	DWORD m_dwCodeSize; //代码段大小
	IMAGE_DATA_DIRECTORY m_PERelocDir; //重定位目录信息
	IMAGE_DATA_DIRECTORY	m_PEImportDir;		//导入表信息
	DWORD m_IATSectionBase; //导入表所在的节的基地址
	DWORD m_IATSectionSize;//导入表所在的节的大小（在修改导入表的时候，需要直接改变这个页的访问属性！）

	PIMAGE_SECTION_HEADER	m_pSecHeader;		//第一个SECTION结构体指针

	LPBYTE m_pFileImageBuffer; //将PE文件映像进行内存拷贝后的首字节

	HANDLE m_hFileMapping; //PE内存映射句柄
	LPVOID m_pFileMappingView;
public:
	VOID InitValue();
	BOOL InitPE(TCHAR* strFilePath);
	void OutPutImportInfo();
	PBYTE GetRVASectionName(DWORD dwAddr);
private:
	BOOL IsPE();
	void GetPEInfo();
	void OriginalCloseFileHandle();
};

