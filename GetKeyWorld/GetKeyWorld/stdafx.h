// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
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

// TODO:  在此处引用程序需要的其他头文件
