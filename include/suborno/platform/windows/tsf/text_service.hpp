#pragma once

#include <msctf.h>
#include <wil/com.h>
#include <wil/resource.h>

#include <string>
#include <unordered_map>

#include "suborno/core/engine.hpp"

namespace suborno::platform::windows {

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

   public:
    static HRESULT CreateInstance(REFIID riid, void** ppv);

   private:
    SubornoTextService();
    ~SubornoTextService();

    HRESULT _CommitString(ITfContext* pContext, const std::wstring& text);

    // COM reference counting
    ULONG m_refCount = 0;

    // TSF interfaces
    wil::com_ptr<ITfThreadMgr> m_threadMgr;
    DWORD m_clientId = TF_CLIENTID_NULL;

    // Per-context composition state
    struct ContextState {
        std::string rawInput;  // UTF-8 Roman input
    };
    std::unordered_map<ITfContext*, ContextState> m_contexts;
};

}  // namespace suborno::platform::windows
