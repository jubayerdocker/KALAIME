#pragma once

#include <msctf.h>
#include <wil/com.h>
#include <wil/resource.h>

#include <string>
#include <unordered_map>

#include "suborno/core/engine.hpp"

namespace suborno::platform::windows {

// CLSID for the Suborno TSF text service
// {4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}
extern const CLSID CLSID_SubornoTextService;

// Language profile GUID
// {7A1B2C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D}
extern const GUID GUID_LANGUAGE_PROFILE_SUBORNO_BENGALI;

// Category GUID for text input processor
// {8F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}
extern const GUID CATID_TEXT_INPUT_PROCESSOR_SUBORNO;

// TSF Text Service implementation
class SubornoTextService final : public ITfTextInputProcessor,
                                 public ITfKeyEventSink,
                                 public ITfThreadMgrEventSink {
   public:
    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    // ITfTextInputProcessor
    STDMETHOD(Activate)(ITfThreadMgr* pThreadMgr, TfClientId clientId) override;
    STDMETHOD(Deactivate)() override;

    // ITfKeyEventSink
    STDMETHOD(OnKeyDown)(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                         BOOL* pbEaten) override;
    STDMETHOD(OnKeyUp)(ITfContext* pContext, WPARAM wParam, LPARAM lParam, BOOL* pbEaten) override;
    STDMETHOD(OnTestKeyDown)(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                             BOOL* pbEaten) override;
    STDMETHOD(OnTestKeyUp)(ITfContext* pContext, WPARAM wParam, LPARAM lParam,
                           BOOL* pbEaten) override;

    // ITfThreadMgrEventSink
    STDMETHOD(OnInitDocumentMgr)(ITfDocumentMgr* pDocMgr) override;
    STDMETHOD(OnUninitDocumentMgr)(ITfDocumentMgr* pDocMgr) override;
    STDMETHOD(OnSetFocus)(ITfContext* pContext, ITfContext* pPrevContext, DWORD dwFlags) override;

   public:
    static HRESULT CreateInstance(REFIID riid, void** ppv);

   private:
    SubornoTextService();
    ~SubornoTextService();

    HRESULT _AdviseSink(ITfContext* pContext);
    HRESULT _UnadviseSink(ITfContext* pContext);
    HRESULT _OnKeyDown(ITfContext* pContext, WPARAM wParam, LPARAM lParam, BOOL* pbEaten);
    HRESULT _CommitString(ITfContext* pContext, const std::wstring& text);
    HRESULT _SetComposition(ITfContext* pContext, const std::wstring& reading,
                            const std::wstring& result);

    // COM reference counting
    ULONG m_refCount = 0;

    // TSF interfaces
    wil::com_ptr<ITfThreadMgr> m_threadMgr;
    wil::com_ptr<ITfSource> m_threadMgrSource;
    TfClientId m_clientId = TF_CLIENTID_NULL;

    // Per-context composition state
    struct ContextState {
        wil::com_ptr<ITfComposition> composition;
        DWORD editCookie = TF_INVALID_COOKIE;
        DWORD sinkCookie = TF_INVALID_COOKIE;
        std::string rawInput;  // UTF-8 Roman input
        bool hasComposition = false;
    };
    std::unordered_map<ITfContext*, ContextState> m_contexts;
};

}  // namespace suborno::platform::windows
