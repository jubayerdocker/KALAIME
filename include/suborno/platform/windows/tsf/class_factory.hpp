#pragma once

#include <unknwn.h>
#include <wil/com.h>

namespace suborno::platform::windows {

class SubornoClassFactory : public IClassFactory {
   public:
    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    // IClassFactory
    STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppv) override;
    STDMETHOD(LockServer)(BOOL fLock) override;

    static HRESULT CreateInstance(REFIID riid, void** ppv);

   private:
    ULONG m_refCount = 0;
    static ULONG s_serverLockCount;
};

}  // namespace suborno::platform::windows
