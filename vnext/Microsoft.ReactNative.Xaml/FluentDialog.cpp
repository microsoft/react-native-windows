#include "pch.h"
#include <windows.h>
#include <stdlib.h>
#include "XamlApplication.h"
#include "resource.h"
#include "DialogFrame.xaml.h"

// Ensure the including PE file has an import reference to
// the WindowsAppSDK runtime DLL and thus gets loaded when
// the including PE file gets loaded.

#define MICROSOFT_WINDOWSAPPSDK_UNDOCKEDREGFREEWINRT_AUTO_INITIALIZE_LOADLIBRARY

STDAPI WindowsAppRuntime_EnsureIsLoaded();

// TODO: Pull this in from the package instead.
namespace Microsoft::Windows::Foundation::UndockedRegFreeWinRT
{
    struct AutoInitialize
    {
        AutoInitialize()
        {
            ::OutputDebugString(L"AutoInitialize happening...");
            // Load the Windows App SDK runtime DLL. The only reason this could fail
            // is if the loading application using WinAppSDK/SelfContained has a
            // damaged self-contained configuration of the Windows App SDK's runtime.

            // Define MICROSOFT_WINDOWSAPPSDK_UNDOCKEDREGFREEWINRT_AUTO_INITIALIZE_LOADLIBRARY
            // if Microsoft.WindowsAppRuntime.dll is DelayLoad'd and thus we need
            // to explicitly load the DLL at runtime. Otherwise we can implicitly
            // link the library and get an import reference causing Windows to
            // resolve our import at load-time.
#if defined(MICROSOFT_WINDOWSAPPSDK_UNDOCKEDREGFREEWINRT_AUTO_INITIALIZE_LOADLIBRARY)
            static HMODULE dll{ LoadLibraryExW(L"Microsoft.WindowsAppRuntime.dll", nullptr, 0) };
            if (!dll)
            {
                const auto lastError{ GetLastError() };
                DebugBreak();
                exit(HRESULT_FROM_WIN32(lastError));
            }
#else
            (void)WindowsAppRuntime_EnsureIsLoaded();
#endif
        }
    };
    static AutoInitialize g_autoInitialize;
}

namespace winrt
{
    using namespace Microsoft::UI;
    using namespace Microsoft::UI::Dispatching;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Hosting;
}

winrt::DispatcherQueueController g_dispatcherQueueController{ nullptr };
winrt::DesktopWindowXamlSource g_desktopWindowXamlSource{ nullptr };

// Custom window procedure for the dialog
LRESULT CALLBACK FluentDialogWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        g_desktopWindowXamlSource = winrt::DesktopWindowXamlSource();
        g_desktopWindowXamlSource.Initialize(winrt::GetWindowIdFromWindow(hwnd));
        winrt::Button b{};
        b.Content(winrt::box_value(L"I am button"));
        g_desktopWindowXamlSource.Content(winrt::make<winrt::Microsoft::ReactNative::Xaml::implementation::DialogFrame>());
        g_desktopWindowXamlSource.SiteBridge().MoveAndResize({ 0,0,300,300 });
        break;
    }
    case WM_SIZE:
        if (g_desktopWindowXamlSource)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            g_desktopWindowXamlSource.SiteBridge().MoveAndResize({ rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top });
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

extern "C" void ShowFluentDialog(HWND hwnd)
{
    winrt::DispatcherQueueController dqc{ nullptr };
    auto dq = winrt::DispatcherQueue::GetForCurrentThread();

    if (!dq)
    {
        g_dispatcherQueueController = winrt::DispatcherQueueController::CreateOnCurrentThread();
    }

    winrt::Microsoft::ReactNative::Xaml::implementation::XamlApplication::EnsureCreated();
    

    // Register window class with custom window procedure
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = FluentDialogWndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"FluentDialogClass";
    RegisterClassEx(&wcex);

    // Calculate the center position relative to the parent window
    RECT parentRect;
    GetWindowRect(hwnd, &parentRect);
    int parentWidth = parentRect.right - parentRect.left;
    int parentHeight = parentRect.bottom - parentRect.top;
    int dialogWidth = 400;
    int dialogHeight = 250;
    int x = parentRect.left + (parentWidth - dialogWidth) / 2;
    int y = parentRect.top + (parentHeight - dialogHeight) / 2;

    // Create the window with dialog-style properties
    HWND dialogHwnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,  // Modal dialog styling
        L"FluentDialogClass",
        L"Notepad",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,   // Dialog-style window
        x, y,                                 // Centered position
        dialogWidth, dialogHeight,
        hwnd,                                 // Parent window
        NULL,                                 // No menu
        GetModuleHandle(NULL),
        NULL);

    if (dialogHwnd)
    {
        // Disable the parent window to make this behave modally
        EnableWindow(hwnd, FALSE);
        
        // Show the window
        ShowWindow(dialogHwnd, SW_SHOW);
        UpdateWindow(dialogHwnd);

        // Modal message loop
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0))
        {
            // Don't use IsDialogMessage here as we're not using a dialog template
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Re-enable the parent window when dialog is closed
        EnableWindow(hwnd, TRUE);
        SetForegroundWindow(hwnd);
    }

    // TODO: Find a good time to clean up the DQC.  Maybe we'll need to have other DLL exports to init and deinit a thread.
    dqc = nullptr;
}