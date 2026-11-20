#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>

#include <atomic>
#include <string>

#include "suborno/platform/windows/tsf/class_factory.hpp"
#include "suborno/platform/windows/tsf/guids.hpp"
#include "suborno/platform/windows/tsf/text_service.hpp"

namespace {

std::atomic<long> g_dllRefCount{0};

// Registry helpers
HRESULT _SetRegistryKeyValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& valueName,
                             const std::wstring& value) {
    HKEY hKey = nullptr;
    LONG result = ::RegCreateKeyExW(hRoot, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                    KEY_SET_VALUE, nullptr, &hKey, nullptr);
    if (result != ERROR_SUCCESS) {
        return HRESULT_FROM_WIN32(result);
    }

    result = ::RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ,
                              reinterpret_cast<const BYTE*>(value.c_str()),
                              static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
    ::RegCloseKey(hKey);

    return result == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(result);
}

HRESULT _DeleteRegistryKey(HKEY hRoot, const std::wstring& subKey) {
    LONG result = ::SHDeleteKeyW(hRoot, subKey.c_str());
    return result == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(result);
}

std::wstring _GuidToString(const GUID& guid) {
    wchar_t szGuid[64] = {};
    ::StringFromGUID2(guid, szGuid, 64);
    return std::wstring(szGuid);
}

}  // namespace

STDAPI DllCanUnloadNow() { return g_dllRefCount.load() == 0 ? S_OK : S_FALSE; }

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if (ppv == nullptr) {
        return E_POINTER;
    }
    *ppv = nullptr;

    if (rclsid == CLSID_SubornoTextService) {
        return suborno::platform::windows::SubornoClassFactory::CreateInstance(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllRegisterServer() {
    const std::wstring clsidStr = _GuidToString(CLSID_SubornoTextService);
    const std::wstring profileGuidStr = _GuidToString(GUID_LANGUAGE_PROFILE_SUBORNO_BENGALI);
    const std::wstring catidStr = _GuidToString(CATID_TEXT_INPUT_PROCESSOR_SUBORNO);

    // Register CLSID
    std::wstring clsidKey = L"CLSID\\" + clsidStr;
    _SetRegistryKeyValue(HKEY_CLASSES_ROOT, clsidKey, L"", L"Suborno Bengali IME Text Service");
    _SetRegistryKeyValue(HKEY_CLASSES_ROOT, clsidKey, L"ThreadingModel", L"Apartment");

    // Register InprocServer32
    std::wstring inprocKey = clsidKey + L"\\InprocServer32";
    wchar_t modulePath[MAX_PATH] = {};
    ::GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
    _SetRegistryKeyValue(HKEY_CLASSES_ROOT, inprocKey, L"", modulePath);
    _SetRegistryKeyValue(HKEY_CLASSES_ROOT, inprocKey, L"ThreadingModel", L"Apartment");

    // Register language profile
    std::wstring profileKey = L"Software\\Microsoft\\CTF\\TIP\\" + clsidStr +
                              L"\\LanguageProfile\\0x00000045\\" + profileGuidStr;
    _SetRegistryKeyValue(HKEY_LOCAL_MACHINE, profileKey, L"", L"Suborno Bengali (Bangladesh)");
    _SetRegistryKeyValue(HKEY_LOCAL_MACHINE, profileKey, L"Description",
                         L"Suborno Bengali Input Method Editor");
    _SetRegistryKeyValue(HKEY_LOCAL_MACHINE, profileKey, L"TIP", clsidStr);

    // Register category
    std::wstring catKey = L"CLSID\\{" + catidStr + L"}\\ImplCategories\\{" + catidStr + L"}";
    _SetRegistryKeyValue(HKEY_CLASSES_ROOT, catKey, L"", L"");

    g_dllRefCount.fetch_add(1);
    return S_OK;
}

STDAPI DllUnregisterServer() {
    const std::wstring clsidStr = _GuidToString(CLSID_SubornoTextService);
    const std::wstring profileGuidStr = _GuidToString(GUID_LANGUAGE_PROFILE_SUBORNO_BENGALI);
    const std::wstring catidStr = _GuidToString(CATID_TEXT_INPUT_PROCESSOR_SUBORNO);

    // Unregister language profile
    std::wstring profileKey = L"Software\\Microsoft\\CTF\\TIP\\" + clsidStr +
                              L"\\LanguageProfile\\0x00000045\\" + profileGuidStr;
    _DeleteRegistryKey(HKEY_LOCAL_MACHINE, profileKey);

    // Unregister category
    std::wstring catKey = L"CLSID\\{" + catidStr + L"}\\ImplCategories\\{" + catidStr + L"}";
    _DeleteRegistryKey(HKEY_CLASSES_ROOT, catKey);

    // Unregister InprocServer32
    std::wstring inprocKey = L"CLSID\\" + clsidStr + L"\\InprocServer32";
    _DeleteRegistryKey(HKEY_CLASSES_ROOT, inprocKey);

    // Unregister CLSID
    _DeleteRegistryKey(HKEY_CLASSES_ROOT, L"CLSID\\" + clsidStr);

    g_dllRefCount.fetch_sub(1);
    return S_OK;
}

STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
    if (bInstall) {
        return DllRegisterServer();
    }
    return DllUnregisterServer();
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hInstance);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
