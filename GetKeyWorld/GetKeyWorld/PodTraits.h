//--------------------------------------------------------------------
// 文件名:		PodTraits.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年1月12日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _POD_TRAITS_H_
#define _POD_TRAITS_H_

#include "Inlines.h"

// TPodTraits

template<typename TYPE>
class TPodTraits
{
public:
	static size_t GetHash(const TYPE& value);
	static bool Equal(const TYPE& v1, const TYPE& v2);
};

template<>
class TPodTraits<int>
{
public:
	static size_t GetHash(const int& value)
	{
		return size_t(value);
	}

	static bool Equal(const int& v1, const int& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<unsigned int>
{
public:
	static size_t GetHash(const unsigned int& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned int& v1, const unsigned int& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<fast_string>
{
public:
	static size_t GetHash(const fast_string& value)
	{
		return GetHashValueCase(value.c_str());
	}

	static bool Equal(const fast_string& v1, const fast_string& v2)
	{
		return strcmp(v1.c_str(), v2.c_str()) == 0;
	}
};

//template<>
//class TPodTraits<int64_t>
//{
//public:
//	static size_t GetHash(const int64_t& value)
//	{
//		return size_t(value);
//	}
//
//	static bool Equal(const int64_t& v1, const int64_t& v2)
//	{
//		return (v1 == v2);
//	}
//};

//template<>
//class TPodTraits<uint64_t>
//{
//public:
//	static size_t GetHash(const uint64_t& value)
//	{
//		return size_t(value);
//	}
//
//	static bool Equal(const uint64_t& v1, const uint64_t& v2)
//	{
//		return (v1 == v2);
//	}
//};

template<>
class TPodTraits<short>
{
public:
	static size_t GetHash(const short& value)
	{
		return size_t(value);
	}

	static bool Equal(const short& v1, const short& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<unsigned short>
{
public:
	static size_t GetHash(const unsigned short& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned short& v1, const unsigned short& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<char>
{
public:
	static size_t GetHash(const char& value)
	{
		return size_t(value);
	}

	static bool Equal(const char& s1, const char& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<unsigned char>
{
public:
	static size_t GetHash(const unsigned char& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned char& s1, const unsigned char& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<void*>
{
public:
	static size_t GetHash(const void*& value)
	{
		return size_t(value);
	}

	static bool Equal(const void*& s1, const void*& s2)
	{
		return (s1 == s2);
	}
};

//template<>
//class TPodTraits<PERSISTID>
//{
//public:
//	static size_t GetHash(const PERSISTID& value)
//	{
//		return size_t(value.nIdent + value.nSerial);
//	}
//
//	static bool Equal(const PERSISTID& s1, const PERSISTID& s2)
//	{
//		return (s1 == s2);
//	}
//};

template<>
class TPodTraits<const char*>
{
public:
	static size_t GetHash(const char* value)
	{
		return GetHashValueCase(value);
	}

	static bool Equal(const char* s1, const char* s2)
	{
		return strcmp(s1, s2) == 0;
	}
};

template<>
class TPodTraits<const wchar_t*>
{
public:
	static size_t GetHash(const wchar_t* value)
	{
		return GetHashValueCaseW(value);
	}

	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return wcscmp(s1, s2) == 0;
	}
};

template<typename TYPE>
class TPodTraitsCI : public TPodTraits<TYPE>
{
};

template<>
class TPodTraitsCI<const char*>
{
public:
	static size_t GetHash(const char* value)
	{
		return GetHashValue(value);
	}

	static bool Equal(const char* s1, const char* s2)
	{
		return stricmp(s1, s2) == 0;
	}
};

template<>
class TPodTraitsCI<const wchar_t*>
{
public:
	static size_t GetHash(const wchar_t* value)
	{
		return GetHashValueW(value);
	}

	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return wcsicmp(s1, s2) == 0;
	}
};

#endif // _POD_TRAITS_H_
