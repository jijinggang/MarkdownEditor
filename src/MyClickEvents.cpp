#include "stdafx.h"
#include "MyClickEvents.h"
#include<string>
#include"./Util.h"


//{{{url_encode
static unsigned char hexchars[] = "0123456789ABCDEF";

/**
* \brief 对url特殊字符进行编码
* \param s 输入字符串
* \param len 输入字符串长度
* \param new_length 输出字符串长度
* \return 输出编码后的url字符串，这段内存再使用完成以后需要释放
*/
char *url_encode(const char *s, int len, int *new_length)
{
	register int x, y;
	unsigned char *str;

	str = (unsigned char *)malloc(3 * len + 1);
	for (x = 0, y = 0; len--; x++, y++)
	{
		str[y] = (unsigned char)s[x];
		if (str[y] == ' ')
		{
			str[y] = '+';
		}
		else if ((str[y] < '0' && str[y] != '-' && str[y] != '.')
			|| (str[y] < 'A' && str[y] > '9')
			|| (str[y] > 'Z' && str[y] < 'a' && str[y] != '_')
			|| (str[y] > 'z'))
		{
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char)s[x] >> 4];
			str[y] = hexchars[(unsigned char)s[x] & 15];
		}
	}
	str[y] = '\0';
	if (new_length) {
		*new_length = y;
	}
	return ((char *)str);
}

void url_encode(std::string &s)
{
	char *buf = url_encode(s.c_str(), s.length(), NULL);
	if (buf)
	{
		s = buf;
		free(buf);
	}
}
//字符串转换整数  
static inline int htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

	return (value);
}

/**
* \brief url字符串解码
* \param str 待解码的字符串，同时也作为输出
* \param len 待解码字符串的长度
* \return 解码以后的字符串长度
*/
int url_decode(char *str, int len)
{
	char *dest = str;
	char *data = str;

	while (len--) {
		if (*data == '+')
			*dest = ' ';
		else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
			*dest = (char)htoi(data + 1);
			data += 2;
			len -= 2;
		}
		else
			*dest = *data;
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}

/**
* \brief url字符串解码
* \param str 待解码的字符串，同时也作为输出
*/
void url_decode(std::string &str)
{
	char buf[2048];
	if (str.length() >= 2048)
		return;
	strcpy(buf, str.c_str());
	url_decode(buf, str.length());
	str = buf;
}

//}}}}}end



CMyClickEvents::CMyClickEvents()
{
	
}



CMyClickEvents::~CMyClickEvents()
{
}

void CMyClickEvents::SetContext(IHTMLDocument2* doc, const char*dir) {
	_pHtmlDoc2 = doc;
	_currentDirectory = dir;
}

// IUnknown
/*
bool isUrlAllowed(char* inUrl) {
	bool ret = false;
	std::string url =  inUrl;
	if (0 == wcscmp(allowedUrl_, L"nohttp")) {
		// Disallow: http://, https:// - case insensitive.
		if (0 == url.compare(0, wcslen(L"http://"), L"http://")
			|| 0 == url.compare(0, wcslen(L"https://"), L"https://"))
		{
			ret = false;
		}
		else {
			ret = true;
		}
	}
	else {
		if (0 == url.compare(0, wcslen(allowedUrl_), allowedUrl_)) {
			ret = true;
		}
		else {
			ret = false;
		}
	}
	delete[] url_lower;
	return ret;
}*/

bool isImageFile(const string& file) {
	std::string ext = Util::GetFileExt(file);
	if (ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "png" || ext == "gif" || ext == "tga")
		return true;
	return false;
}

void openUrl(char* href, const char* dir) {
	//if (!browserWindow_->IsUrlAllowed(href, _countof(href))) {
	std::string url = href;
	url_decode(url);
	if (isImageFile(url))
		return;
	if (url.find_first_of("file:///") == 0) {
		url = url.substr(sizeof("file:///") - 1);
	}else if (url.find_first_of("about:") == 0) {
		url = url.substr(sizeof("about:")-1);
		url = dir  + url;
	}
	//url = "C:/Users/Ourpalm/Desktop/1.md";
	ShellExecute(0, _T("open"), url.c_str(), 0,  dir , SW_SHOWNORMAL);
	int err = GetLastError();
	//}
}
HRESULT STDMETHODCALLTYPE CMyClickEvents::Invoke(
	/* [in] */ DISPID dispId,
	/* [in] */ REFIID riid,
	/* [in] */ LCID lcid,
	/* [in] */ WORD wFlags,
	/* [out][in] */ DISPPARAMS *pDispParams,
	/* [out] */ VARIANT *pVarResult,
	/* [out] */ EXCEPINFO *pExcepInfo,
	/* [out] */ UINT *puArgErr) {
	// When returning a result, you must check whether pVarResult
	// is not NULL and initialize it using VariantInit(). If it's
	// NULL then it doesn't expect a result.
	if (riid != IID_NULL)
		return DISP_E_UNKNOWNINTERFACE;
	pExcepInfo = 0;
	puArgErr = 0;
	HRESULT hr;

	CComPtr<IHTMLWindow2> htmlWindow2 = NULL;
	hr = _pHtmlDoc2->get_parentWindow(
		static_cast<IHTMLWindow2**>(&htmlWindow2));
	if (FAILED(hr) || !htmlWindow2) {
		//LOG_WARNING << "CMyClickEvents::Invoke() failed: "
		//	"IHTMLDocument2->get_parentWindow() failed";
		return S_OK;
	}
	CComPtr<IHTMLEventObj> htmlEvent = NULL;
	hr = htmlWindow2->get_event(&htmlEvent);
	if (FAILED(hr) || !htmlEvent) {
		//LOG_WARNING << "CMyClickEvents::Invoke() failed: "
		//	"IHTMLWindow2->get_event() failed";
		return S_OK;
	}
	CComPtr<IHTMLElement> htmlElement = NULL;
	hr = htmlEvent->get_srcElement(&htmlElement);
	if (FAILED(hr) || !htmlElement) {
		//LOG_WARNING << "CMyClickEvents::Invoke() failed: "
		//	"IHTMLEventObj->get_srcElement() failed";
		return S_OK;
	}
	_bstr_t hrefAttr(L"href");
	VARIANT attrValue;
	VariantInit(&attrValue);
	hr = htmlElement->getAttribute(hrefAttr, 0, &attrValue);
	if (FAILED(hr)) {
		//LOG_WARNING << "CMyClickEvents::Invoke() failed: "
		//	"IHTMLElement->getAttribute() failed";
		return S_OK;
	}
	if (attrValue.vt == VT_BSTR && attrValue.bstrVal) {
		// Href attribute found. When not found vt is VT_NULL.
		// Maximum url length in IE is 2084, see:
		// http://support.microsoft.com/kb/208427
		char* href = _com_util::ConvertBSTRToString(attrValue.bstrVal);

		//
		VARIANT eventReturn;
		VariantInit(&eventReturn);
		eventReturn.vt = VT_BOOL;
		eventReturn.boolVal = VARIANT_FALSE;
		htmlEvent->put_returnValue(eventReturn);
		openUrl(href, _currentDirectory.c_str());
	}
	return S_OK;
}