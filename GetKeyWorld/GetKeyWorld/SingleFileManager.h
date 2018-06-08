#ifndef SINGLE_FILE_MANAGER_H_
#define SINGLE_FILE_MANAGER_H_
//每一次双击某一个栏目都会创建一个单独的对话框和相关数据，通过这个对他们进行管理

#include "PodHashMap.h"
#include "SingleFileErrorShow.h"

#define GetSingleFileContainer(HWND) g_pSingleFileManager->GetSingleFileContailer(HWND)
#define RemoveHWNDFromManager(HWND) g_pSingleFileManager->RemoveSingleFileFromManager(HWND)
#define AddContailer2Manager(HWND, p) g_pSingleFileManager->AddSingleFileContailer(HWND, p)

class SingleViewContainer;

class SingleFileManager
{
public:
	static SingleViewContainer* GetSingleFileContailer(HWND hWnd);
	static void RemoveSingleFileFromManager(HWND hWnd);
	static void AddSingleFileContailer(HWND hWnd, SingleViewContainer* pContainer);

	typedef TPodHashMap<unsigned int, SingleViewContainer*> self_type;
	typedef self_type::iterator self_type_iter;

	self_type m_HashPod;

	SingleFileManager();
	~SingleFileManager();
};


extern SingleFileManager* g_pSingleFileManager;

#endif