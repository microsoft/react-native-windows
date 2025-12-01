#include "pch.h"
#include "resource.h"

#include <windows.h>
#include <windowsx.h>

// Disabled until we have a 3rd party story for custom components
// #include "AutolinkedNativeModules.g.h"

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

#include <DesktopWindowBridge.h>
#include "App.xaml.h"
#include "AutoDraw.h"
#include "NativeModules.h"
#include "ReactPropertyBag.h"


#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.interop.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.interop.h>

winrt::Microsoft::UI::Dispatching::DispatcherQueueController g_liftedDispatcherQueueController{nullptr};
winrt::Microsoft::UI::Composition::Compositor g_liftedCompositor{nullptr};

/**
 * This ImageHandler will accept images with a uri using the ellipse protocol and render an ellipse image
 *
 *   <Image
 *      style={{width: 400, height: 200}}
 *      source={{uri: 'customimage://test'}}
 *   />
 *
 * This allows applications to provide custom image rendering pipelines.
 */
struct EllipseImageHandler
    : winrt::implements<EllipseImageHandler, winrt::Microsoft::ReactNative::Composition::IUriImageProvider> {
  bool CanLoadImageUri(winrt::Microsoft::ReactNative::IReactContext /*context*/, winrt::Windows::Foundation::Uri uri) {
    return uri.SchemeName() == L"ellipse";
  }

  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
  GetImageResponseAsync(
      const winrt::Microsoft::ReactNative::IReactContext &context,
      const winrt::Microsoft::ReactNative::Composition::ImageSource &imageSource) {
    co_return winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactoryImageResponse(
        [uri = imageSource.Uri(), size = imageSource.Size(), scale = imageSource.Scale(), context](
            const winrt::Microsoft::ReactNative::IReactContext & /*reactContext*/,
            const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compositionContext)
            -> winrt::Microsoft::ReactNative::Composition::Experimental::IBrush {
          auto compositor = winrt::Microsoft::ReactNative::Composition::Experimental::
              MicrosoftCompositionContextHelper::InnerCompositor(compositionContext);
          auto drawingBrush = compositionContext.CreateDrawingSurfaceBrush(
              size,
              winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
              winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
          POINT pt;
          Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingBrush, scale, &pt);
          auto renderTarget = autoDraw.GetRenderTarget();

          winrt::com_ptr<ID2D1SolidColorBrush> brush;
          renderTarget->CreateSolidColorBrush({1.0f, 0.0f, 0.0f, 1.0f}, brush.put());
          renderTarget->DrawEllipse(
              {{(pt.x + size.Width / 2) / scale, (pt.y + size.Height / 2) / scale},
               (size.Width / 2) / scale,
               (size.Height / 2) / scale},
              brush.get());

          return drawingBrush;
        });
  }
};





constexpr auto WindowDataProperty = L"WindowData";

int RunPlayground(int showCmd);

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort = 9229;

  std::wstring m_bundleFile;
  winrt::Microsoft::ReactNative::ReactNativeIsland m_compRootView{nullptr};
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_bridge{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  bool m_sizeToContent{false};
  bool m_forceRTL{false};
  LONG m_height{0};
  LONG m_width{0};

  WindowData() {}

  static WindowData *GetFromWindow(HWND hwnd) {
    auto data = reinterpret_cast<WindowData *>(GetProp(hwnd, WindowDataProperty));
    return data;
  }

  winrt::Microsoft::ReactNative::ReactNativeHost Host() noexcept {
    if (!m_host) {
      m_host = winrt::Microsoft::ReactNative::ReactNativeHost();
      m_host.InstanceSettings(InstanceSettings());
    }

    return m_host;
  }
  winrt::Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept {
    if (!m_instanceSettings) {
      m_instanceSettings = winrt::Microsoft::ReactNative::ReactInstanceSettings();
      m_instanceSettings.UseDirectDebugger(true);
      m_instanceSettings.UseFastRefresh(true);
    }

    return m_instanceSettings;
  }

  void ApplyConstraintsForContentSizedWindow(winrt::Microsoft::ReactNative::LayoutConstraints &constraints) {
    constraints.MinimumSize = {300, 300};
    constraints.MaximumSize = {1000, 1000};
  }

  LRESULT OnCommand(HWND hwnd, int id, HWND /* hwndCtl*/, UINT) {
    switch (id) {
      case IDM_OPENJSFILE: {
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_OPENJSBUNDLEBOX), hwnd, &Bundle);

        if (!m_bundleFile.empty()) {
          PCWSTR appName = (m_bundleFile == LR"(Samples\rntester)") ? L"RNTesterApp" : L"Bootstrap";

          WCHAR appDirectory[MAX_PATH];
          GetModuleFileNameW(NULL, appDirectory, MAX_PATH);
          PathCchRemoveFileSpec(appDirectory, MAX_PATH);

          auto host = Host();
          // Disable until we have a 3rd party story for custom components
          // RegisterAutolinkedNativeModulePackages(host.PackageProviders()); // Includes any autolinked modules

          host.InstanceSettings().JavaScriptBundleFile(m_bundleFile);

          host.InstanceSettings().BundleRootPath(
              std::wstring(L"file://").append(appDirectory).append(L"\\Bundle\\").c_str());
          host.InstanceSettings().UseDeveloperSupport(true);

          // Some of the images in RNTester require a user-agent header to properly fetch
          winrt::Microsoft::ReactNative::HttpSettings::SetDefaultUserAgent(
              host.InstanceSettings(), L"React Native Windows Playground");




          winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
              host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

          winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
          viewOptions.ComponentName(appName);

          if (!m_compRootView) {
            m_compRootView = winrt::Microsoft::ReactNative::ReactNativeIsland(g_liftedCompositor);

            // By setting the compositor here we opt into using the new architecture.
            winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositor(
                InstanceSettings(), g_liftedCompositor);



            m_bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
                g_liftedCompositor, winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));

            if (m_forceRTL) {
              m_bridge.LayoutDirectionOverride(winrt::Microsoft::UI::Content::ContentLayoutDirection::RightToLeft);
            }

            auto appContent = m_compRootView.Island();

            m_bridge.Connect(appContent);
            m_bridge.Show();

            m_compRootView.ScaleFactor(ScaleFactor(hwnd));
            winrt::Microsoft::ReactNative::LayoutConstraints constraints;
            constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
            constraints.MaximumSize =
                constraints.MinimumSize = {m_width / ScaleFactor(hwnd), m_height / ScaleFactor(hwnd)};

            if (m_sizeToContent) {
              ApplyConstraintsForContentSizedWindow(constraints);

              // Disable user sizing of the hwnd
              ::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
              m_compRootView.SizeChanged([hwnd, props = InstanceSettings().Properties()](
                                             auto /*sender*/,
                                             const winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs &args) {
                auto compositor =
                    winrt::Microsoft::ReactNative::Composition::CompositionUIService::GetCompositor(props);
                auto async = compositor.RequestCommitAsync();
                async.Completed([hwnd, size = args.Size()](
                                    auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus /*asyncStatus*/) {
                  RECT rcClient, rcWindow;
                  GetClientRect(hwnd, &rcClient);
                  GetWindowRect(hwnd, &rcWindow);

                  SetWindowPos(
                      hwnd,
                      nullptr,
                      0,
                      0,
                      static_cast<int>(size.Width) + rcClient.left - rcClient.right + rcWindow.right - rcWindow.left,
                      static_cast<int>(size.Height) + rcClient.top - rcClient.bottom + rcWindow.bottom - rcWindow.top,
                      SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                });
              });
            }
            m_compRootView.Arrange(constraints, {0, 0});

            m_bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);
          }

          // Nudge the ReactNativeHost to create the instance and wrapping context
          host.ReloadInstance();

          m_compRootView.ReactViewHost(
              winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));
        }

        break;
      }
      case IDM_NEWWINDOW: {
        std::thread playgroundThread{([]() {
          // For subsequent RN windows do not use the web debugger by default,
          // since one instance can be connected to it at a time.
          RunPlayground(SW_SHOW);
        })};
        playgroundThread.detach();
        break;
      }
      case IDM_ABOUT:
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, &About);
        break;
      case IDM_EXIT:
        PostQuitMessage(0);
        break;
      case IDM_REFRESH:
        Host().ReloadInstance();
        break;
      case IDM_SETTINGS:
        DialogBoxParam(s_instance, MAKEINTRESOURCE(IDD_SETTINGSBOX), hwnd, &Settings, reinterpret_cast<INT_PTR>(this));
        break;
      case IDM_UNLOAD: {
        auto async = Host().UnloadInstance();
        async.Completed([&, uidispatch = InstanceSettings().UIDispatcher()](
                            auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          asyncStatus;
          OutputDebugStringA("Instance Unload completed\n");

          uidispatch.Post([&]() {
            if (m_bridge) {
              m_bridge.Close();
              m_bridge = nullptr;
            }
          });
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
        });
        m_compRootView = nullptr;
        m_instanceSettings = nullptr;
        m_host = nullptr;
      } break;
      case IDM_TOGGLE_LAYOUT_DIRECTION: {
        if (m_bridge) {
          m_bridge.LayoutDirectionOverride(
              (m_forceRTL) ? winrt::Microsoft::UI::Content::ContentLayoutDirection::LeftToRight
                           : winrt::Microsoft::UI::Content::ContentLayoutDirection::RightToLeft);
        }
        m_forceRTL = !m_forceRTL;
      }
      case IDM_SETPROPS: {
        m_compRootView.SetProperties([](const winrt::Microsoft::ReactNative::IJSValueWriter &writer) {
          static int value = 123;
          writer.WriteObjectBegin();
          winrt::Microsoft::ReactNative::WriteProperty(writer, L"testProp1", value++);
          winrt::Microsoft::ReactNative::WriteProperty(writer, L"testProp2", L"value2");
          writer.WriteObjectEnd();
        });
      }
    }

    return 0;
  }

  float ScaleFactor(HWND hwnd) noexcept {
    return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
  }

  void UpdateSize(HWND hwnd) noexcept {
    RECT rc;
    if (GetClientRect(hwnd, &rc)) {
      if (m_height != (rc.bottom - rc.top) || m_width != (rc.right - rc.left)) {
        m_height = rc.bottom - rc.top;
        m_width = rc.right - rc.left;

        if (m_compRootView) {
          winrt::Windows::Foundation::Size size{m_width / ScaleFactor(hwnd), m_height / ScaleFactor(hwnd)};
          if (!IsIconic(hwnd)) {
            winrt::Microsoft::ReactNative::LayoutConstraints constraints;
            constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
            constraints.MinimumSize = constraints.MaximumSize = size;
            if (m_sizeToContent) {
              ApplyConstraintsForContentSizedWindow(constraints);
            }
            m_compRootView.Arrange(constraints, {0, 0});
          }
        }
      }
    }
  }

  /// Message handler for about box.
  static INT_PTR CALLBACK About(HWND hwnd, UINT message, WPARAM wparam, LPARAM /* lparam */) noexcept {
    switch (message) {
      case WM_INITDIALOG:
        return TRUE;

      case WM_COMMAND:
        if (LOWORD(wparam) == IDOK || LOWORD(wparam) == IDCANCEL) {
          EndDialog(hwnd, LOWORD(wparam));
          return TRUE;
        }
        break;
    }

    return FALSE;
  }

  static constexpr std::wstring_view g_bundleFiles[] = {LR"(Samples\rntester)",     LR"(Samples\accessible)",
                                                        LR"(Samples\callbackTest)", LR"(Samples\calculator)",
                                                        LR"(Samples\click)",        LR"(Samples\control)",
                                                        LR"(Samples\flexbox)",      LR"(Samples\focusTest)",
                                                        LR"(Samples\geosample)",    LR"(Samples\image)",
                                                        LR"(Samples\index)",        LR"(Samples\nativeFabricComponent)",
                                                        LR"(Samples\mouse)",        LR"(Samples\scrollViewSnapSample)",
                                                        LR"(Samples\simple)",       LR"(Samples\text)",
                                                        LR"(Samples\textinput)",    LR"(Samples\ticTacToe)",
                                                        LR"(Samples\view)",         LR"(Samples\debugTest01)"};

  static INT_PTR CALLBACK Bundle(HWND hwnd, UINT message, WPARAM wparam, LPARAM /*lparam*/) noexcept {
    switch (message) {
      case WM_INITDIALOG: {
        HWND hwndListBox = GetDlgItem(hwnd, IDC_JSBUNDLELIST);
        for (int i = 0; i < _countof(g_bundleFiles); i++) {
          SendMessage(hwndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(g_bundleFiles[i].data()));
        }
        return TRUE;
      }
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDOK: {
            HWND hwndListBox = GetDlgItem(hwnd, IDC_JSBUNDLELIST);
            int selectedItem = static_cast<int>(SendMessage(hwndListBox, LB_GETCURSEL, 0, 0));
            if (0 <= selectedItem && selectedItem < _countof(g_bundleFiles)) {
              auto self = GetFromWindow(GetParent(hwnd));
              self->m_bundleFile = g_bundleFiles[selectedItem];
            }
          }
            [[fallthrough]];
          case IDCANCEL:
            EndDialog(hwnd, LOWORD(wparam));
            return TRUE;
        }
        break;
    }

    return FALSE;
  }

  static INT_PTR CALLBACK Settings(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    switch (message) {
      case WM_INITDIALOG: {
        auto boolToCheck = [](bool b) { return b ? BST_CHECKED : BST_UNCHECKED; };
        auto self = reinterpret_cast<WindowData *>(lparam);
        CheckDlgButton(hwnd, IDC_FASTREFRESH, boolToCheck(self->InstanceSettings().UseFastRefresh()));
        CheckDlgButton(hwnd, IDC_DIRECTDEBUGGER, boolToCheck(self->InstanceSettings().UseDirectDebugger()));
        CheckDlgButton(hwnd, IDC_BREAKONNEXTLINE, boolToCheck(self->InstanceSettings().DebuggerBreakOnNextLine()));
        CheckDlgButton(hwnd, IDC_SIZETOCONTENT, boolToCheck(self->m_sizeToContent));

        auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
        SetWindowTextW(portEditControl, std::to_wstring(self->InstanceSettings().DebuggerPort()).c_str());
        SendMessageW(portEditControl, (UINT)EM_SETLIMITTEXT, (WPARAM)5, (LPARAM)0);

        return TRUE;
      }
      case WM_COMMAND: {
        switch (LOWORD(wparam)) {
          case IDOK: {
            auto self = GetFromWindow(GetParent(hwnd));
            self->InstanceSettings().UseFastRefresh(IsDlgButtonChecked(hwnd, IDC_FASTREFRESH) == BST_CHECKED);
            self->InstanceSettings().UseDirectDebugger(IsDlgButtonChecked(hwnd, IDC_DIRECTDEBUGGER) == BST_CHECKED);
            self->InstanceSettings().DebuggerBreakOnNextLine(
                IsDlgButtonChecked(hwnd, IDC_BREAKONNEXTLINE) == BST_CHECKED);
            self->m_sizeToContent = (IsDlgButtonChecked(hwnd, IDC_SIZETOCONTENT) == BST_CHECKED);

            WCHAR buffer[6] = {};
            auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
            GetWindowTextW(portEditControl, buffer, ARRAYSIZE(buffer));

            try {
              auto port = std::stoi(buffer);
              if (port > UINT16_MAX)
                port = defaultDebuggerPort;
              self->InstanceSettings().DebuggerPort(static_cast<uint16_t>(port));
            } catch (const std::out_of_range &) {
              self->InstanceSettings().DebuggerPort(defaultDebuggerPort);
            } catch (const std::invalid_argument &) {
              // Don't update the debugger port if the new value can't be parsed
              // (E.g. includes letters or symbols).
            }
          }
            [[fallthrough]];
          case IDCANCEL:
            EndDialog(hwnd, LOWORD(wparam));
            return true;
        }
        break;
      }
    }

    return FALSE;
  }
};

extern "C" IMAGE_DOS_HEADER __ImageBase;
HINSTANCE WindowData::s_instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  auto windowData = WindowData::GetFromWindow(hwnd);

  switch (message) {
    case WM_COMMAND: {
      return WindowData::GetFromWindow(hwnd)->OnCommand(
          hwnd, LOWORD(wparam), reinterpret_cast<HWND>(lparam), HIWORD(wparam));
    }
    case WM_DESTROY: {
      auto data = WindowData::GetFromWindow(hwnd);
      // Before we shutdown the application - gracefully unload the ReactNativeHost instance
      bool shouldPostQuitMessage = true;
      if (data->m_host) {
        shouldPostQuitMessage = false;

        winrt::Microsoft::ReactNative::ReactPropertyBag properties(data->m_host.InstanceSettings().Properties());

        properties.Remove(winrt::Microsoft::ReactNative::ReactPropertyId<
                          winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
            L"ReactNative.Composition", L"CompositionContext"});

        auto async = data->m_host.UnloadInstance();
        async.Completed([host = data->m_host](auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          asyncStatus;
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
          host.InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
        });
        data->m_compRootView = nullptr;
        data->m_instanceSettings = nullptr;
        data->m_host = nullptr;
      }

      delete WindowData::GetFromWindow(hwnd);
      SetProp(hwnd, WindowDataProperty, 0);
      if (shouldPostQuitMessage) {
        PostQuitMessage(0);
      }
      return 0;
    }
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
      windowData = static_cast<WindowData *>(cs->lpCreateParams);
      WINRT_ASSERT(windowData);
      SetProp(hwnd, WindowDataProperty, reinterpret_cast<HANDLE>(windowData));
      break;
    }
    case WM_WINDOWPOSCHANGED: {
      windowData->UpdateSize(hwnd);

      winrt::Microsoft::ReactNative::ReactNotificationService rns(windowData->InstanceSettings().Notifications());
      winrt::Microsoft::ReactNative::ForwardWindowMessage(rns, hwnd, message, wparam, lparam);
      break;
    }
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

constexpr PCWSTR c_windowClassName = L"MS_REACTNATIVE_PLAYGROUND_COMPOSITION";

int RunPlayground(int showCmd) {
  constexpr PCWSTR appName = L"React Native Playground (Composition)";

  auto windowData = std::make_unique<WindowData>();
  HWND hwnd = CreateWindow(
      c_windowClassName,
      appName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      nullptr,
      nullptr,
      WindowData::s_instance,
      windowData.get());

  WINRT_VERIFY(hwnd);

  windowData.release();

  ShowWindow(hwnd, showCmd);
  UpdateWindow(hwnd);
  SetFocus(hwnd);

  g_liftedDispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  g_liftedDispatcherQueueController.ShutdownQueue();

  return 0;
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
  // Island-support: Call init_apartment to initialize COM and WinRT for the thread.
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  WNDCLASSEXW wcex = {};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = &WndProc;
  wcex.cbClsExtra = DLGWINDOWEXTRA;
  wcex.cbWndExtra = sizeof(WindowData *);
  wcex.hInstance = WindowData::s_instance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAYGROUND_COMPOSITION);
  wcex.lpszClassName = c_windowClassName;
  wcex.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_ICON1));
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

  // Create a Lifted (WinAppSDK) DispatcherQueue for this thread.  This is needed for
  // Microsoft.UI.Composition, Content, and Input APIs.
  g_liftedDispatcherQueueController =
      winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();
  g_liftedCompositor = winrt::Microsoft::UI::Composition::Compositor();

  // Island-support: Create our custom Xaml App object. This is needed to properly use the controls and metadata
  // in Microsoft.ui.xaml.controls.dll.
  auto playgroundApp{winrt::make<winrt::Playground::implementation::App>()};

  return RunPlayground(showCmd);
}
