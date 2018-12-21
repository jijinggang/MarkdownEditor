#include "StdAfx.h"
#include "Util.h"
#include "sundown/markdown.h"
#include "sundown/html.h"
#include <sstream>
#include <string>
#include <cctype>  
#include <algorithm>  

using namespace std;
Util::Util(void)
{
}


Util::~Util(void)
{
}

string Util::GetFileExt(const string& strPathFile) {
	size_t nFind = strPathFile.find_last_of('.');
	if (nFind < 0)
	{
		return "";
	}
	string s = strPathFile.substr(nFind + 1);
	std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
	return s;
}
string Util::GetFilePath(const string& strPathFile, bool linuxPath){
	size_t nFind = strPathFile.find_last_of('\\');
	if (nFind < 0)
	{
		return strPathFile;
	}
	string strPath = strPathFile.substr(0, nFind + 1);
	if (linuxPath){
		strPath = Util::ReplaceAllStr(strPath, "\\", "/");
	}
	return strPath;
}
string Util::GetFileName(const string& strPathFile) {
	size_t nFind = strPathFile.find_last_of('\\');
	if (nFind < 0)
	{
		return strPathFile;
	}
	string strPath = strPathFile.substr(nFind + 1);
	return strPath;
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
	sdhtml_renderer(&callbacks, &options, 0| HTML_TOC);
	markdown = sd_markdown_new(MKDEXT_NO_INTRA_EMPHASIS|MKDEXT_TABLES|MKDEXT_AUTOLINK|MKDEXT_FENCED_CODE|MKDEXT_STRIKETHROUGH|MKDEXT_SUPERSCRIPT| MKDEXT_SPACE_HEADERS| MKDEXT_LAX_SPACING, 16, &callbacks, &options);

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

string Util::ReadStringFile(CFile& file){
	int len = (int)file.GetLength();
	char* buf = new char[len + 2];
	buf[len] = 0;
	buf[len + 1] = 0;
	file.Read(buf,len);
	string str = Util::AnyToANSI(buf, len);
	delete []buf;
	return str;
}

string Util::ReadStringFile(LPCTSTR strFileName)
{
	CFile file;
	if(!file.Open(strFileName, CFile::modeRead))
		return "";
	return ReadStringFile(file);
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
		strResult = Util::AnyToANSI(szMsg,nLength) ;
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

string&  Util::ReplaceAllStr(string& str,const string& old_value,const string&   new_value)   
{   
    for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
        if(   (pos=str.find(old_value,pos))!=string::npos   )   
            str.replace(pos,old_value.length(),new_value);   
        else   break;   
    }   
    return   str;   
}   

int Util::IsTextUTF8(const char* str,long length)
{
	int i;
	int nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	bool bAllAscii=true; //如果全部都是ASCII, 说明不是UTF-8
	for(i=0;i<length;i++)
	{
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if(chr>=0x80)
			{
				if(chr>=0xFC&&chr<=0xFD)
					nBytes=6;
				else if(chr>=0xF8)
					nBytes=5;
				else if(chr>=0xF0)
					nBytes=4;
				else if(chr>=0xE0)
					nBytes=3;
				else if(chr>=0xC0)
					nBytes=2;
				else
				{
					return false;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return false;
			}
			nBytes--;
		}
	}

	if( nBytes > 0 ) //违返规则
	{
		return false;
	}

	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
	{
		return false;
	}
	return true;
}

string Util::AnyToANSI(const char* str, int len){
	if(IsTextUTF8(str, len)){
		return UTF8ToANSI(str);
	}else{
		return str;
	}
}