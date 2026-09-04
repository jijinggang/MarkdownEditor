#pragma once
#include "mshtml.h"
#include <string>
class CMyClickEvents :
	public IDispatch
{
private:
	CComPtr<IHTMLDocument2> _pHtmlDoc2; // AddRef'd; swapped on document change
	std::string     _currentDirectory;
	long _refCount;
public:
	CMyClickEvents();
	void SetContext(IHTMLDocument2* doc, const char*dir);
	~CMyClickEvents();


	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid, void** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	// IDispatch
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
		/* [out] */ UINT *pctinfo) {
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo) {
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(
		/* [in] */ REFIID riid,
		/* [in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [out] */ DISPID *rgDispId) {
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispId,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr);
};
