// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "HandleMenuOption.h"
#include "BasicUtil.h"
#include "ListViewRelated.h"
#include "AnalysisLog.h"
#include "Inlines.h"
#include "StringPod.h"
#include "StringTraits.h"
#include "MyFile.h"
#include "Commdlg.h"
#include "GetKeyWorld.h"


#include "util_func.h"
#include "FastStr.h"


#define WM_MY_MESSAGE (WM_USER+100)

enum
{
	DLG_INIT_INFO,
};

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
