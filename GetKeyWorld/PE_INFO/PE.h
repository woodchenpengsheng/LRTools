#pragma once
class CPE
{
public:
	CPE();
	~CPE();
public:
	HANDLE m_hFile;	//PE�ļ����
	PIMAGE_DOS_HEADER m_pDosHeader; //Dosͷ
	PIMAGE_NT_HEADERS m_pNtHeader; //Ntͷ
	DWORD m_dwFileSize; //PE�ļ���С
	DWORD m_dwImageSize; //PE�ļ��ڴ�ӳ���С
	DWORD m_dwMemAlign; //PE�ڴ����ߴ�
	DWORD m_dwSizeOfHeader; //���˽����ݵ������ڴ�ӳ��Ĵ�С
	DWORD m_dwSectionNum; //�ڵ�����

	DWORD m_dwFileAlign;//�ļ�����
	DWORD m_dwImageBase;//����װ�ڻ���ַ
	DWORD m_dwPEOEP; //���
	DWORD m_dwCodeBase; //������ʼ
	DWORD m_dwCodeSize; //����δ�С
	IMAGE_DATA_DIRECTORY m_PERelocDir; //�ض�λĿ¼��Ϣ
	IMAGE_DATA_DIRECTORY	m_PEImportDir;		//�������Ϣ
	DWORD m_IATSectionBase; //��������ڵĽڵĻ���ַ
	DWORD m_IATSectionSize;//��������ڵĽڵĴ�С�����޸ĵ�����ʱ����Ҫֱ�Ӹı����ҳ�ķ������ԣ���

	PIMAGE_SECTION_HEADER	m_pSecHeader;		//��һ��SECTION�ṹ��ָ��

	LPBYTE m_pFileImageBuffer; //��PE�ļ�ӳ������ڴ濽��������ֽ�

	HANDLE m_hFileMapping; //PE�ڴ�ӳ����
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

