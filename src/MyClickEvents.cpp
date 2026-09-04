#include "stdafx.h"
#include "MyClickEvents.h"
#include <string>
#include <algorithm>
#include "./Util.h"


//{{{url_encode
static unsigned char hexchars[] = "0123456789ABCDEF";

/**
* \brief ��url�����ַ����б���
* \param s �����ַ���
* \param len �����ַ�������
* \param new_length ����ַ�������
* \return ���������url�ַ���������ڴ���ʹ������Ժ���Ҫ�ͷ�
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
//�ַ���ת������  
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
* \brief url�ַ�������
* \param str ��������ַ�����ͬʱҲ��Ϊ���
* \param len �������ַ����ĳ���
* \return �����Ժ���ַ�������
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
* \brief url�ַ�������
* \param str ��������ַ�����ͬʱҲ��Ϊ���
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
	: _refCount(0)
{

}



CMyClickEvents::~CMyClickEvents()
{
}

// IUnknown
HRESULT STDMETHODCALLTYPE CMyClickEvents::QueryInterface(
	REFIID riid, void** ppvObject) {
	if (ppvObject == NULL)
		return E_POINTER;
	*ppvObject = NULL;
	if (riid == IID_IUnknown || riid == IID_IDispatch) {
		*ppvObject = static_cast<IDispatch*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE CMyClickEvents::AddRef(void) {
	return ++_refCount;
}
ULONG STDMETHODCALLTYPE CMyClickEvents::Release(void) {
	const ULONG r = --_refCount;
	if (r == 0)
		delete this;
	return r;
}

void CMyClickEvents::SetContext(IHTMLDocument2* doc, const char*dir) {
	_pHtmlDoc2 = doc; // CComPtr releases the previous document
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

// Only links with a safe scheme (or relative paths) may be opened.
static bool IsSafeHref(const string& url)
{
	string lower = url;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	// reject control characters anywhere in the URL
	for (string::size_type i = 0; i < lower.size(); i++) {
		if (lower[i] < 0x20)
			return false;
	}
	// scheme = [a-z][a-z0-9+.-]*":" ; no colon means a relative path
	const string::size_type colon = lower.find(':');
	if (colon == string::npos)
		return true;
	if (colon == 0)
		return false;
	for (string::size_type i = 0; i < colon; i++) {
		const char c = lower[i];
		if (i == 0) {
			if (!(c >= 'a' && c <= 'z'))
				return false;
		} else if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
			|| c == '+' || c == '.' || c == '-')) {
			return false;
		}
	}
	static const char* allowed[] = { "http:", "https:", "file:", "mailto:" };
	for (int i = 0; i < (int)(sizeof(allowed)/sizeof(allowed[0])); i++) {
		if (lower.compare(0, colon + 1, allowed[i]) == 0)
			return true;
	}
	return false;
}

void openUrl(const string& href, const char* dir) {
	std::string url = href;
	url_decode(url);
	if (isImageFile(url))
		return;
	// prefix match (find_first_of was a character-set test, not a substring test)
	if (url.compare(0, 8, "file:///") == 0) {
		url = url.substr(8);
	}else if (url.compare(0, 6, "about:") == 0) {
		url = url.substr(6);
		url = dir + url;
	}
	if (!IsSafeHref(url))
		return;
	// internal strings are UTF-8; widen only at the OS boundary
	const INT_PTR h = (INT_PTR)ShellExecuteW(0, L"open", CA2W(url.c_str(), CP_UTF8), 0, CA2W(dir, CP_UTF8), SW_SHOWNORMAL);
	if (h <= 32)
		TRACE("openUrl: ShellExecute failed (result=%d)\n", (int)h);
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
	if (!_pHtmlDoc2)
		return S_OK;

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
	CComBSTR hrefAttr(L"href");
	CComVariant attrValue; // frees the BSTR on destruction
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
		const string href = Util::Utf16ToUtf8(attrValue.bstrVal, SysStringLen(attrValue.bstrVal));

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