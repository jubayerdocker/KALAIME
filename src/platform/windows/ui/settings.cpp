#include "suborno/platform/windows/ui/settings.hpp"

#include <commctrl.h>

#include <string>

namespace suborno::platform::windows::ui {

namespace {

const wchar_t* kStatusWindowClass = L"SubornoStatusWindow";
const wchar_t* kSettingsDialogClass = L"SubornoSettingsDialog";

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

}  // namespace

void ShowSettings(HWND hOwner) {
    DLGPROC dlgProc = SettingsDialogProc;
    DialogBoxParamW(nullptr, MAKEINTRESOURCEW(101), hOwner, dlgProc, 0);
}

ATOM RegisterStatusWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    };
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wcex.lpszClassName = kStatusWindowClass;
    return RegisterClassExW(&wcex);
}

HWND CreateStatusWindow(HINSTANCE hInstance, HWND hParent) {
    return CreateWindowExW(0, kStatusWindowClass, L"Suborno", WS_CHILD | WS_VISIBLE, 0, 0, 100, 20,
                           hParent, nullptr, hInstance, nullptr);
}

void UpdateStatusDisplay(HWND hStatusWnd, const wchar_t* status) {
    if (hStatusWnd != nullptr && status != nullptr) {
        SetWindowTextW(hStatusWnd, status);
    }
}

}  // namespace suborno::platform::windows::ui
