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

std::string utf16_to_utf8(const std::wstring& utf16) {
    if (utf16.empty()) {
        return "";
    }
    int size_needed = ::WideCharToMultiByte(
        CP_UTF8, 0, utf16.c_str(), static_cast<int>(utf16.size()), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) {
        return "";
    }
    std::string utf8(size_needed, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), static_cast<int>(utf16.size()), &utf8[0],
                          size_needed, nullptr, nullptr);
    return utf8;
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

    // Get the source interface for advising sinks
    HRESULT hr = m_threadMgr->QueryInterface(IID_PPV_ARGS(&m_threadMgrSource));
    if (FAILED(hr)) {
        return hr;
    }

    // Advise thread manager sink
    m_threadMgrSinkCookie = TF_INVALID_COOKIE;
    hr = m_threadMgrSource->AdviseSink(IID_ITfThreadMgrEventSink,
                                       static_cast<ITfThreadMgrEventSink*>(this),
                                       &m_threadMgrSinkCookie);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP SubornoTextService::Deactivate() {
    // Unadvise thread manager sink
    if (m_threadMgrSource) {
        m_threadMgrSource->UnadviseSink(m_threadMgrSinkCookie);
        m_threadMgrSource.reset();
    }

    m_threadMgr.reset();
    m_clientId = TF_CLIENTID_NULL;

    // Clean up all context states
    m_contexts.clear();

    return S_OK;
}

STDMETHODIMP SubornoTextService::OnInitDocumentMgr(ITfDocumentMgr* pDocMgr) { return S_OK; }

STDMETHODIMP SubornoTextService::OnUninitDocumentMgr(ITfDocumentMgr* pDocMgr) { return S_OK; }

STDMETHODIMP SubornoTextService::OnSetFocus(ITfContext* pContext, ITfContext* pPrevContext,
                                            DWORD dwFlags) {
    if (pContext != nullptr) {
        _AdviseSink(pContext);
    }
    if (pPrevContext != nullptr) {
        _UnadviseSink(pPrevContext);
    }
    return S_OK;
}

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
    return _OnKeyDown(pContext, wParam, lParam, pbEaten);
}

STDMETHODIMP SubornoTextService::OnKeyUp(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                         BOOL* pbEaten) {
    if (pbEaten == nullptr) {
        return E_INVALIDARG;
    }
    *pbEaten = FALSE;
    return S_OK;
}

HRESULT SubornoTextService::_AdviseSink(ITfContext* pContext) {
    if (pContext == nullptr) {
        return E_INVALIDARG;
    }

    // Check if already advised
    if (m_contexts.find(pContext) != m_contexts.end()) {
        return S_OK;
    }

    ContextState state;
    state.hasComposition = false;

    // Advise context sink for edit events
    wil::com_ptr<ITfSource> source;
    HRESULT hr = pContext->QueryInterface(IID_PPV_ARGS(&source));
    if (FAILED(hr)) {
        return hr;
    }

    // Store context state
    m_contexts.emplace(pContext, state);
    return S_OK;
}

HRESULT SubornoTextService::_UnadviseSink(ITfContext* pContext) {
    if (pContext == nullptr) {
        return E_INVALIDARG;
    }

    auto it = m_contexts.find(pContext);
    if (it != m_contexts.end()) {
        m_contexts.erase(it);
    }

    return S_OK;
}

HRESULT SubornoTextService::_OnKeyDown(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                                       BOOL* pbEaten) {
    auto it = m_contexts.find(pContext);
    if (it == m_contexts.end()) {
        *pbEaten = FALSE;
        return S_OK;
    }

    ContextState& state = it->second;

    // Handle special keys
    if (wParam == VK_BACK) {
        // Backspace
        if (!state.rawInput.empty()) {
            state.rawInput.pop_back();
            if (state.rawInput.empty()) {
                state.hasComposition = false;
            } else {
                _CommitString(
                    pContext,
                    utf8_to_utf16(
                        suborno::layouts::phonetic::detail::transliterate_primary(state.rawInput)));
            }
            *pbEaten = TRUE;
            return S_OK;
        }
        *pbEaten = FALSE;
        return S_OK;
    }

    if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
        // Commit or cancel composition
        if (!state.rawInput.empty()) {
            const auto result =
                suborno::layouts::phonetic::detail::transliterate_primary(state.rawInput);
            _CommitString(pContext, utf8_to_utf16(result));
            state.rawInput.clear();
            state.hasComposition = false;
            *pbEaten = TRUE;
            return S_OK;
        }
        *pbEaten = FALSE;
        return S_OK;
    }

    if (wParam == VK_SPACE) {
        // Space commits current composition
        if (!state.rawInput.empty()) {
            const auto result =
                suborno::layouts::phonetic::detail::transliterate_primary(state.rawInput);
            _CommitString(pContext, utf8_to_utf16(result));
            state.rawInput.clear();
            state.hasComposition = false;
            *pbEaten = TRUE;
            return S_OK;
        }
        *pbEaten = FALSE;
        return S_OK;
    }

    // Handle character input (A-Z, a-z)
    std::string input;
    if (wParam >= 'A' && wParam <= 'Z') {
        input.push_back(static_cast<char>(wParam | 0x20));  // lowercase
    } else if (wParam >= 'a' && wParam <= 'z') {
        input.push_back(static_cast<char>(wParam));
    } else {
        *pbEaten = FALSE;
        return S_OK;
    }

    // Process through phonetic engine
    state.rawInput += input;
    const auto result = suborno::layouts::phonetic::detail::transliterate_primary(state.rawInput);

    _CommitString(pContext, utf8_to_utf16(result));
    *pbEaten = TRUE;
    return S_OK;
}

HRESULT SubornoTextService::_CommitString(ITfContext* pContext, const std::wstring& text) {
    if (pContext == nullptr || text.empty()) {
        return S_OK;
    }

    // Get selection range
    TF_SELECTION selection = {};
    ULONG fetched = 0;
    HRESULT hr = pContext->GetSelection(m_clientId, TF_DEFAULT_SELECTION, 1, &selection, &fetched);
    if (FAILED(hr) || fetched == 0) {
        return hr;
    }

    wil::com_ptr<ITfRange> range(selection.range);

    // Set text in range
    hr = range->SetText(m_clientId, 0, text.c_str(), static_cast<LONG>(text.size()));
    if (FAILED(hr)) {
        return hr;
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
    service->Release();  // Release initial reference from new
    return hr;
}

}  // namespace suborno::platform::windows
