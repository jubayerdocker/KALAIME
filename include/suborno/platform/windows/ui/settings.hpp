#pragma once

#include <Windows.h>

namespace suborno::platform::windows::ui {

// Main settings dialog procedure
INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Show the settings dialog
void ShowSettings(HWND hOwner);

// Register window class for IME status indicator
ATOM RegisterStatusWindowClass(HINSTANCE hInstance);

// Create the IME status window
HWND CreateStatusWindow(HINSTANCE hInstance, HWND hParent);

// Update the status display
void UpdateStatusDisplay(HWND hStatusWnd, const wchar_t* status);

}  // namespace suborno::platform::windows::ui
