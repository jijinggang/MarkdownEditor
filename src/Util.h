#pragma once
#include <string>
using namespace std;
class Util
{

public:
	Util(void);
	~Util(void);
	static string Text2Md(const string& str);
	static string GetFilePath(const string& strPathFile, bool linuxPath = false);
	static string GetExePath();
	static string ReadStringFile(LPCTSTR strFileName);
	static string ReadStringFile(CFile& file);
	static BOOL WriteStringFile(LPCTSTR strFileName, CString& strContent);
	static bool LoadStringRes(int idRes, LPCTSTR lpszType,string& strResult);
	static wchar_t * ANSIToUnicode( const char* str );
	static char * UnicodeToANSI( const wchar_t *str );
	static wchar_t * UTF8ToUnicode( const char* str );
	static char * UnicodeToUTF8( const wchar_t *str );
	static string ANSIToUTF8(const char* str);
	static string UTF8ToANSI(const char* str);
	static string&  ReplaceAllStr(string& str,const string& old_value,const string&   new_value);
	static int Util::IsTextUTF8(const char* str,long length);
	static string AnyToANSI(const char* str, int len);
};

