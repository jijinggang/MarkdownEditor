#pragma once
#include <string>
using namespace std;
class Util
{

public:
	Util(void);
	~Util(void);
	static string Text2Md(const string& str);
	static string GetFileExt(const string& strPathFile);
	static string GetFilePath(const string& strPathFile, bool linuxPath = false);
	static string GetFileName(const string& strPathFile);
	static string GetExePath();
	static string ReadStringFile(LPCTSTR strFileName);
	static string ReadStringFile(CFile& file);
	static BOOL WriteStringFile(LPCTSTR strFileName, const string& utf8Content);
	static bool LoadStringRes(int idRes, LPCTSTR lpszType,string& strResult);
	// UTF-8 <-> UTF-16 conversion. The whole app stores text as UTF-8;
	// conversion to wide chars happens only at the Win32/UI boundary.
	static string Utf16ToUtf8(const wchar_t* str, int len = -1);
	static wstring Utf8ToUtf16(const char* str, int len = -1);
	static bool IsValidUtf8(const char* str, int len);
	static string&  ReplaceAllStr(string& str,const string& old_value,const string&   new_value);
	static int IsTextUTF8(const char* str,long length);
};
