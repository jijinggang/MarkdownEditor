#include "StdAfx.h"
#include "Util.h"
#include "sundown/markdown.h"
#include "sundown/html.h"
#include <sstream>
#include <string>

using namespace std;
Util::Util(void)
{
}


Util::~Util(void)
{
}

string Util::GetExePath(){
	TCHAR szBuffer[MAX_PATH];
	::GetModuleFileName(NULL, szBuffer, sizeof(szBuffer) / sizeof(TCHAR));
	string strPath = szBuffer;
	size_t nFind = strPath.find_last_of('\\');
	if(nFind < 0)
	{
		return "";
	}
	return strPath.substr(0,nFind + 1);
}



string Util::Text2Md(const string& str){
#define READ_UNIT 1024
#define OUTPUT_UNIT 64
	struct buf *ob;
	struct sd_callbacks callbacks;
	struct html_renderopt options;
	struct sd_markdown *markdown;
	int ret;

	ob = bufnew(OUTPUT_UNIT);
	sdhtml_renderer(&callbacks, &options, 0);
	markdown = sd_markdown_new(MKDEXT_TABLES|MKDEXT_AUTOLINK, 16, &callbacks, &options);

	sd_markdown_render(ob, (const uint8_t*)(str.c_str()), str.size(), markdown);
	//sd_markdown_render(ob, ib->data, ib->size, markdown);
	sd_markdown_free(markdown);

	/* writing the result to stdout */
	ret = fwrite(ob->data, 1, ob->size, stdout);
	std::stringbuf sb;
	sb.sputn((const char*)ob->data, ob->size);

	/* cleanup */
	bufrelease(ob);	
	
	
	return sb.str();
}

const int MAX_BUFF = 102400;

CString Util::ReadStringFile(LPCTSTR strFileName)
{
	CFile file;
	if(!file.Open(strFileName, CFile::modeRead))
		return "";
	CString strRst;
	TCHAR buff[MAX_BUFF] = {0};
	while(true){
		UINT uRead = file.Read(buff,MAX_BUFF);
		strRst.Append(buff , uRead);
		if(uRead < MAX_BUFF)
			break;
	}
	return Util::UTF8ToANSI(strRst).c_str();
}



BOOL Util::WriteStringFile(LPCTSTR strFileName, CString& strContent)
{
	CFile file;
	if(!file.Open(strFileName, CFile::modeCreate|CFile::modeWrite))
		return FALSE;
	//file.WriteString(strContent);
	file.Write(strContent.GetBuffer(), strContent.GetLength());
	return TRUE;
}


bool Util::LoadStringRes(int idRes, LPCTSTR lpszType, string& strResult)
{
		HRSRC hRes = ::FindResource(NULL , MAKEINTRESOURCE(idRes), lpszType);
		if(! hRes)
			return false;
		HGLOBAL hData = ::LoadResource(NULL, hRes);
		void* pData = ::LockResource(hData);
		int nLength = ::SizeofResource(NULL, hRes);
		char* szMsg = new char[nLength + 1];
		memcpy(szMsg, pData, nLength);
		szMsg[nLength] = '\0';
		UnlockResource(pData);
		strResult = Util::UTF8ToANSI(szMsg) ;
		delete []szMsg;
		return true;
	return false;
}

wchar_t* Util::ANSIToUnicode( const char* str )
{
      int    textlen ;
      wchar_t * result;
      textlen = MultiByteToWideChar( CP_ACP, 0, str,-1,    NULL,0 );  
      result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));  
      memset(result,0,(textlen+1)*sizeof(wchar_t));  
      MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );  
      return    result;  
}
char* Util::UnicodeToANSI( const wchar_t *str )
{
      char * result;
      int textlen;
      // wide char to multi char
      textlen = WideCharToMultiByte( CP_ACP,    0,    str,    -1,    NULL, 0, NULL, NULL );
      result =(char *)malloc((textlen+1)*sizeof(char));
      memset( result, 0, sizeof(char) * ( textlen + 1 ) );
      WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
      return result;
}
wchar_t* Util::UTF8ToUnicode( const char* str )
{
      int    textlen ;
      wchar_t * result;
      textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1,    NULL,0 );  
      result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));  
      memset(result,0,(textlen+1)*sizeof(wchar_t));  
      MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );  
      return    result;  
}
char * Util::UnicodeToUTF8( const wchar_t *str )
{
      char * result;
      int textlen;
      // wide char to multi char
      textlen = WideCharToMultiByte( CP_UTF8,    0,    str,    -1,    NULL, 0, NULL, NULL );
      result =(char *)malloc((textlen+1)*sizeof(char));
      memset(result, 0, sizeof(char) * ( textlen + 1 ) );
      WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
      return result;
} 

string Util::ANSIToUTF8(const char* str){
	wchar_t* unicode = ANSIToUnicode(str);
	char* utf8 = UnicodeToUTF8(unicode);
	string strUtf8(utf8);
	free(unicode);
	free(utf8);
	return strUtf8;
}
string Util::UTF8ToANSI(const char* str){
	if(str[0] == -17 && str[1] == -69 && str[2] == -65){ //对Windows上的UTF8头特殊处理，忽略掉
		str = str+3;
	}
	wchar_t* unicode = UTF8ToUnicode(str);
	char* ansi = UnicodeToANSI(unicode);
	string strAnsi = ansi;
	free(unicode);
	free(ansi);
	return strAnsi;
}
