#pragma once
#include "mshtml.h"
#include <string>
class CMyClickEvents :
	public IDispatch
{
private:
	IHTMLDocument2* _pHtmlDoc2;
	std::string     _currentDirectory;
public:
	CMyClickEvents();
	void SetContext(IHTMLDocument2* doc, const char*dir);
	~CMyClickEvents();


	// IUnknown
	HRESULT STDMETHODCALLTYPE CMyClickEvents::QueryInterface(
		REFIID riid, void** ppvObject) {
		*ppvObject = 0;
		return E_NOTIMPL;
	}
	ULONG STDMETHODCALLTYPE CMyClickEvents::AddRef(void) {
		return 1;
	}
	ULONG STDMETHODCALLTYPE CMyClickEvents::Release(void) {
		return 1;
	}
	// IDispatch
	HRESULT STDMETHODCALLTYPE CMyClickEvents::GetTypeInfoCount(
		/* [out] */ UINT *pctinfo) {
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE CMyClickEvents::GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo) {
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE CMyClickEvents::GetIDsOfNames(
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

