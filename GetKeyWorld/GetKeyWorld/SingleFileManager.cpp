#include "stdafx.h"
#include "SingleFileManager.h"

SingleFileManager* g_pSingleFileManager = new SingleFileManager();
void SingleFileManager::AddSingleFileContailer(HWND hWnd, SingleViewContainer* pContainer)
{
	SingleFileManager::self_type_iter iter = g_pSingleFileManager->m_HashPod.Find((unsigned int)hWnd);
	if (iter != g_pSingleFileManager->m_HashPod.End())
	{
		delete iter.GetData();
		g_pSingleFileManager->m_HashPod.Remove((unsigned int)hWnd);
	}

	g_pSingleFileManager->m_HashPod.Add((unsigned int)hWnd, pContainer);
}

SingleViewContainer* SingleFileManager::GetSingleFileContailer(HWND hWnd)
{
	SingleFileManager::self_type_iter iter = g_pSingleFileManager->m_HashPod.Find((unsigned int)hWnd);
	if (iter == g_pSingleFileManager->m_HashPod.End())
	{
		return NULL;
	}
	else
	{
		return iter.GetData();
	}
}

void SingleFileManager::RemoveSingleFileFromManager(HWND hWnd)
{
	g_pSingleFileManager->m_HashPod.Remove((unsigned int)hWnd);
}

SingleFileManager::SingleFileManager()
{
	m_HashPod.Clear();
}

SingleFileManager::~SingleFileManager()
{
	self_type_iter iter = m_HashPod.Begin();
	while (iter != m_HashPod.End())
	{
		SingleViewContainer*& pContainerData = iter.GetData();
		if (NULL != pContainerData)
		{
			delete pContainerData;
		}
		pContainerData = NULL;
		iter++;
	}
	m_HashPod.Clear();
}