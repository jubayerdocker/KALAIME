#include "suborno/platform/windows/tsf/text_service.hpp"

#include <shlwapi.h>

#include <algorithm>
#include <codecvt>
#include <locale>
#include <string>

#include "../../../layouts/phonetic/phonetic_parser.hpp"
#include "suborno/core/engine.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"
#include "suborno/unicode/bengali.hpp"

namespace suborno::platform::windows {

namespace {

std::wstring utf8_to_utf16(const std::string& utf8) {
    if (utf8.empty()) {
        return L"";
    }
    int size_needed =
        ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
    if (size_needed <= 0) {
        return L"";
    }
    std::wstring utf16(size_needed, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), &utf16[0],
                          size_needed);
    return utf16;
}

}  // namespace

SubornoTextService::SubornoTextService() = default;

SubornoTextService::~SubornoTextService() = default;

STDMETHODIMP SubornoTextService::QueryInterface(REFIID riid, void** ppvObj) {
    if (ppvObj == nullptr) {
        return E_POINTER;
    }
    *ppvObj = nullptr;

    if (riid == IID_IUnknown || riid == IID_ITfTextInputProcessor) {
        *ppvObj = static_cast<ITfTextInputProcessor*>(this);
    } else if (riid == IID_ITfKeyEventSink) {
        *ppvObj = static_cast<ITfKeyEventSink*>(this);
    } else if (riid == IID_ITfThreadMgrEventSink) {
        *ppvObj = static_cast<ITfThreadMgrEventSink*>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) SubornoTextService::AddRef() {
    return static_cast<ULONG>(::InterlockedIncrement(&m_refCount));
}

STDMETHODIMP_(ULONG) SubornoTextService::Release() {
    const ULONG count = static_cast<ULONG>(::InterlockedDecrement(&m_refCount));
    if (count == 0) {
        delete this;
    }
    return count;
}

STDMETHODIMP SubornoTextService::Activate(ITfThreadMgr* pThreadMgr, TfClientId clientId) {
    if (pThreadMgr == nullptr) {
        return E_INVALIDARG;
    }

    m_threadMgr = pThreadMgr;
    m_clientId = clientId;
    return S_OK;
}

STDMETHODIMP SubornoTextService::Deactivate() {
    m_threadMgr.reset();
    m_clientId = TF_CLIENTID_NULL;
    m_contexts.clear();
    return S_OK;
}

STDMETHODIMP SubornoTextService::OnInitDocumentMgr(ITfDocumentMgr* pDocMgr) { return S_OK; }

STDMETHODIMP SubornoTextService::OnUninitDocumentMgr(ITfDocumentMgr* pDocMgr) { return S_OK; }

STDMETHODIMP SubornoTextService::OnTestKeyDown(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                               BOOL* pbEaten) {
    if (pbEaten == nullptr) {
        return E_INVALIDARG;
    }
    *pbEaten = FALSE;
    return S_OK;
}

STDMETHODIMP SubornoTextService::OnTestKeyUp(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                             BOOL* pbEaten) {
    if (pbEaten == nullptr) {
        return E_INVALIDARG;
    }
    *pbEaten = FALSE;
    return S_OK;
}

STDMETHODIMP SubornoTextService::OnKeyDown(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                           BOOL* pbEaten) {
    if (pbEaten == nullptr) {
        return E_INVALIDARG;
    }
    return _CommitString(pContext, utf8_to_utf16(""));
}

STDMETHODIMP SubornoTextService::OnKeyUp(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                         BOOL* pbEaten) {
    if (pbEaten == nullptr) {
        return E_INVALIDARG;
    }
    *pbEaten = FALSE;
    return S_OK;
}

HRESULT SubornoTextService::_CommitString(ITfContext* pContext, const std::wstring& text) {
    if (pContext == nullptr || text.empty()) {
        return S_OK;
    }
    return S_OK;
}

HRESULT SubornoTextService::CreateInstance(REFIID riid, void** ppv) {
    if (ppv == nullptr) {
        return E_POINTER;
    }
    *ppv = nullptr;

    SubornoTextService* service = new (std::nothrow) SubornoTextService();
    if (service == nullptr) {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = service->QueryInterface(riid, ppv);
    service->Release();
    return hr;
}

}  // namespace suborno::platform::windows
