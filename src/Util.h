#pragma once
#include <string>
using namespace std;
class Util
{

public:
	Util(void);
	~Util(void);
	static string Text2Md(const string& str);
	static string GetExePath();
	static CString ReadStringFile(LPCTSTR strFileName);
	static BOOL WriteStringFile(LPCTSTR strFileName, CString& strContent);
	static bool LoadStringRes(int idRes, LPCTSTR lpszType,string& strResult);
	static wchar_t * ANSIToUnicode( const char* str );
	static char * UnicodeToANSI( const wchar_t *str );
	static wchar_t * UTF8ToUnicode( const char* str );
	static char * UnicodeToUTF8( const wchar_t *str );
	static string ANSIToUTF8(const char* str);
	static string UTF8ToANSI(const char* str);
};

