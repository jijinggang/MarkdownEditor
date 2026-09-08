#include "StdAfx.h"
#include "Util.h"
#include "md4c/md4c.h"
#include "md4c/md4c-html.h"
#include <string>
#include <vector>
#include <cctype>
#include <string.h>
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
	if (nFind == string::npos)
	{
		return "";
	}
	string s = strPathFile.substr(nFind + 1);
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}
string Util::GetFilePath(const string& strPathFile, bool linuxPath){
	size_t nFind = strPathFile.find_last_of('\\');
	if (nFind == string::npos)
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
	if (nFind == string::npos)
	{
		return strPathFile;
	}
	string strPath = strPathFile.substr(nFind + 1);
	return strPath;
}
string Util::GetExePath(){
	wchar_t szBuffer[MAX_PATH];
	::GetModuleFileNameW(NULL, szBuffer, sizeof(szBuffer) / sizeof(wchar_t));
	string strPath = Util::Utf16ToUtf8(szBuffer);
	size_t nFind = strPath.find_last_of('\\');
	if(nFind == string::npos)
	{
		return "";
	}
	return strPath.substr(0,nFind + 1);
}



namespace {
struct MdOutput {
	string buf;
};

void MdOutputCallback(const MD_CHAR* data, MD_SIZE size, void* userdata)
{
	static_cast<MdOutput*>(userdata)->buf.append(data, size);
}
}

// md4c percent-escapes every byte >= 0x80 in link/image URLs (its URL escape
// map only allows ASCII alphanumerics and a few punctuation characters), so
// "<img src=CN_DIR/a.png>" comes out as "%E5%9B%BE...". MSHTML decodes %XX with
// the system ANSI code page rather than UTF-8, so the file would not be found
// on non-ASCII paths. Decode the escapes whose byte value is >= 0x80 back to
// raw UTF-8. ASCII escapes (%20 etc.) are left alone, and '%' itself is never
// produced by md4c's escaper, so literal "%XX" in the source text survives.
static void DecodeNonAsciiUrlEscapes(string& s)
{
    static const char hex[] = "0123456789ABCDEF";
    string out;
    out.reserve(s.size());
    string::size_type i = 0;
    const string::size_type n = s.size();
    while (i < n) {
        const unsigned char c = (unsigned char)s[i];
        if (c == '%' && i + 2 < n) {
            const char* hi = strchr(hex, toupper((unsigned char)s[i + 1]));
            const char* lo = strchr(hex, toupper((unsigned char)s[i + 2]));
            // strchr(hex,'\0') would find the terminator; guard against NUL bytes
            if (hi != NULL && lo != NULL && s[i + 1] != '\0' && s[i + 2] != '\0') {
                const unsigned char byte = (unsigned char)((hi - hex) * 16 + (lo - hex));
                if (byte >= 0x80) {
                    out += (char)byte;
                    i += 3;
                    continue;
                }
            }
        }
        out += s[i];
        i++;
    }
    s.swap(out);
}

string Util::Text2Md(const string& str){
	// MD_DIALECT_GITHUB = tables + strikethrough + task lists + permissive
	// autolinks, matching the GFM-style preview CSS. Unlike sundown's
	// HTML_SAFELINK there is no link scheme filtering here; raw HTML still
	// passes through, so StripScriptTags and openUrl()'s scheme allowlist
	// remain the security gates.
	MdOutput out;
	md_html(str.c_str(), (MD_SIZE)str.size(), MdOutputCallback, &out,
		MD_DIALECT_GITHUB, 0);
	DecodeNonAsciiUrlEscapes(out.buf);
	return out.buf;
}

const int MAX_BUFF = 102400;

// Load file content as UTF-8. Detects UTF-8/UTF-16 BOMs; without a BOM,
// valid UTF-8 is taken verbatim, otherwise the bytes are assumed to be in
// the system ANSI code page (legacy GBK files) and converted to UTF-8.
string Util::ReadStringFile(CFile& file){
	const ULONGLONG ullLen = file.GetLength();
	// sane cap: markdown files are small; also avoids int truncation of GetLength()
	if (ullLen > 32ull*1024*1024)
		return "";
	const DWORD len = (DWORD)ullLen;
	std::vector<unsigned char> buf((size_t)len + 2, 0);
	const UINT nRead = file.Read(buf.data(), len);
	if (nRead == 0 && len != 0)
		return "";
	const unsigned char* data = buf.data();

	// UTF-8 BOM
	if (nRead >= 3 && data[0]==0xEF && data[1]==0xBB && data[2]==0xBF)
		return string((const char*)data + 3, nRead - 3);

	// UTF-16LE / UTF-16BE BOM
	if (nRead >= 2 && ((data[0]==0xFF && data[1]==0xFE) || (data[0]==0xFE && data[1]==0xFF)))
	{
		const bool bSwapped = (data[0]==0xFE);
		const int nChars = (int)((nRead - 2)/2);
		std::vector<wchar_t> wbuf((size_t)nChars + 1, 0);
		const unsigned char* p = data + 2;
		for (int i = 0; i < nChars; i++)
		{
			wbuf[i] = bSwapped ? (wchar_t)((p[2*i] << 8) | p[2*i+1])
			                   : (wchar_t)(p[2*i] | (p[2*i+1] << 8));
		}
		return Util::Utf16ToUtf8(wbuf.data(), nChars);
	}

	// No BOM: valid UTF-8 is taken verbatim
	if (Util::IsValidUtf8((const char*)data, nRead))
		return string((const char*)data, nRead);

	// legacy ANSI/GBK -> wide -> UTF-8
	const int wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)data, nRead, NULL, 0);
	if (wlen <= 0)
		return "";
	std::vector<wchar_t> wbuf((size_t)wlen, 0);
	MultiByteToWideChar(CP_ACP, 0, (const char*)data, nRead, wbuf.data(), wlen);
	return Util::Utf16ToUtf8(wbuf.data(), wlen);
}

string Util::ReadStringFile(LPCTSTR strFileName)
{
	CFile file;
	if(!file.Open(strFileName, CFile::modeRead))
		return "";
	return ReadStringFile(file);
}

BOOL Util::WriteStringFile(LPCTSTR strFileName, const string& utf8Content)
{
	CFile file;
	if(!file.Open(strFileName, CFile::modeCreate|CFile::modeWrite))
		return FALSE;
	file.Write(utf8Content.data(), (UINT)utf8Content.size());
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
		if(! pData || nLength <= 0)
			return false;
		strResult.assign((const char*)pData, nLength);
		return true;
}

string Util::Utf16ToUtf8(const wchar_t* str, int len)
{
	if (str == NULL || len == 0)
		return "";
	if (len < 0)
		len = (int)wcslen(str);
	const int utf8len = WideCharToMultiByte(CP_UTF8, 0, str, len, NULL, 0, NULL, NULL);
	if (utf8len <= 0)
		return "";
	string result((size_t)utf8len, '\0');
	if (WideCharToMultiByte(CP_UTF8, 0, str, len, &result[0], utf8len, NULL, NULL) <= 0)
		return "";
	return result;
}

wstring Util::Utf8ToUtf16(const char* str, int len)
{
	if (str == NULL || len == 0)
		return L"";
	if (len < 0)
		len = (int)strlen(str);
	const int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, len, NULL, 0);
	if (wlen <= 0)
		return L"";
	wstring result((size_t)wlen, L'\0');
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, len, &result[0], wlen) <= 0)
		return L"";
	return result;
}

bool Util::IsValidUtf8(const char* str, int len)
{
	if (str == NULL || len <= 0)
		return false;
	return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, len, NULL, 0) > 0;
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
	int nBytes=0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	unsigned char chr;
	bool bAllAscii=true; //���ȫ������ASCII, ˵������UTF-8
	for(i=0;i<length;i++)
	{
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
		{
			if(chr>=0x80)
			{
				// only legal UTF-8 lead bytes: 2/3/4-byte sequences.
				// (5/6-byte forms are illegal and made GBK pairs validate as UTF-8.)
				if(chr>=0xF0)
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
		else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return false;
			}
			nBytes--;
		}
	}

	if( nBytes > 0 ) //Υ������
	{
		return false;
	}

	if( bAllAscii ) //���ȫ������ASCII, ˵������UTF-8
	{
		return false;
	}
	return true;
}
