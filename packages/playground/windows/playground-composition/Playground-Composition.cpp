#include "pch.h"
#include "resource.h"

#include <windows.h>
#include <windowsx.h>

#include <memory>
#include <mutex>

// Disabled until we have a 3rd party story for custom components
// #include "AutolinkedNativeModules.g.h"

#include <winrt/Windows.Foundation.Collections.h>

#include <DispatcherQueue.h>
#include <UIAutomation.h>
#include <windows.ui.composition.interop.h>

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>

#include <DesktopWindowBridge.h>
#include "App.xaml.h"
#include "AutoDraw.h"
#include "NativeModules.h"
#include "ReactPropertyBag.h"

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.interop.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.interop.h>

// Includes from sample-custom-component
#include <winrt/SampleCustomComponent.h>

winrt::Microsoft::UI::Dispatching::DispatcherQueueController g_liftedDispatcherQueueController{nullptr};
winrt::Microsoft::UI::Composition::Compositor g_liftedCompositor{nullptr};

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;

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
  bool CanLoadImageUri(winrt::Microsoft::ReactNative::IReactContext context, winrt::Windows::Foundation::Uri uri) {
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

struct EllipseReactPackageProvider
    : winrt::implements<EllipseReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    // Register ellipse: uri handler for images
    packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddUriImageProvider(
        winrt::make<EllipseImageHandler>());
  }
};

// Have to use TurboModules to override built in modules.. so the standard attributed package provider doesn't work.
struct CompReactPackageProvider
    : winrt::implements<CompReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const & packageBuilder) noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
    RegisterCustomComponent(packageBuilder);
#endif // USE_EXPERIMENTAL_WINUI3
  }
};

winrt::Windows::System::DispatcherQueueController g_dispatcherQueueController{nullptr};
winrt::Windows::UI::Composition::Compositor g_compositor{nullptr};

constexpr auto WindowDataProperty = L"WindowData";

int RunPlayground(int showCmd, bool useWebDebugger);
winrt::Microsoft::ReactNative::IReactPackageProvider CreateStubDeviceInfoPackageProvider() noexcept;

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort = 9229;

  std::wstring m_bundleFile;
  winrt::Microsoft::ReactNative::ReactNativeIsland m_compRootView{nullptr};
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_bridge{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  bool m_useLiftedComposition{true};
  bool m_sizeToContent{false};
  bool m_forceRTL{false};
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};
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

  winrt::Microsoft::UI::WindowId
  CreateChildWindow(winrt::Microsoft::UI::WindowId parentWindowId, LPCWSTR title, int x, int y, int w, int h) {
    LPCWSTR childWindowClassName = L"TestChildWindowClass";

    // Register child window class
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [&childWindowClassName]() {
      WNDCLASSEX childWindowClass = {};

      childWindowClass.cbSize = sizeof(WNDCLASSEX);
      childWindowClass.style = CS_HREDRAW | CS_VREDRAW;
      childWindowClass.lpfnWndProc = ::DefWindowProc;
      childWindowClass.hInstance = GetModuleHandle(nullptr);
      childWindowClass.lpszClassName = childWindowClassName;

      RegisterClassEx(&childWindowClass);
    });

    HWND parentHwnd;
    parentHwnd = winrt::Microsoft::UI::GetWindowFromWindowId(parentWindowId);

    HWND childHwnd = ::CreateWindowEx(
        0 /* dwExStyle */,
        childWindowClassName,
        title,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        x,
        y,
        w,
        h,
        parentHwnd /* hWndParent */,
        nullptr /* hMenu */,
        GetModuleHandle(nullptr),
        nullptr /* lpParam */);

    return winrt::Microsoft::UI::GetWindowIdFromWindow(childHwnd);
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

          host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());

          host.PackageProviders().Append(winrt::SampleCustomComponent::ReactPackageProvider());

          winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
              host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

          winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
          viewOptions.ComponentName(appName);
          auto windowData = WindowData::GetFromWindow(hwnd);

          if (!m_compRootView) {
            if (windowData->m_useLiftedComposition) {
              m_compRootView = winrt::Microsoft::ReactNative::ReactNativeIsland(g_liftedCompositor);
            } else {
              m_compRootView = winrt::Microsoft::ReactNative::ReactNativeIsland();
            }

            if (windowData->m_useLiftedComposition) {
              // By setting the compositor here we opt into using the new architecture.
              winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositor(
                  InstanceSettings(), g_liftedCompositor);

              // Register ellipse:// uri hander for images
              host.PackageProviders().Append(winrt::make<EllipseReactPackageProvider>());

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
                m_compRootView.SizeChanged(
                    [hwnd](auto sender, const winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs &args) {
                      RECT rcClient, rcWindow;
                      GetClientRect(hwnd, &rcClient);
                      GetWindowRect(hwnd, &rcWindow);

                      SetWindowPos(
                          hwnd,
                          nullptr,
                          0,
                          0,
                          static_cast<int>(args.Size().Width) + rcClient.left - rcClient.right + rcWindow.right -
                              rcWindow.left,
                          static_cast<int>(args.Size().Height) + rcClient.top - rcClient.bottom + rcWindow.bottom -
                              rcWindow.top,
                          SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                    });
              }
              m_compRootView.Arrange(constraints, {0, 0});

              m_bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

            } else if (!m_target) {
              // General users of RNW should never set CompositionContext - this is an advanced usage to inject another
              // composition implementation. By using the SystemCompositionContextHelper here, React Native Windows will
              // use System Visuals for its tree.
              winrt::Microsoft::ReactNative::ReactPropertyBag(InstanceSettings().Properties())
                  .Set(
                      winrt::Microsoft::ReactNative::ReactPropertyId<
                          winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
                          L"ReactNative.Composition", L"CompositionContext"},
                      winrt::Microsoft::ReactNative::Composition::Experimental::SystemCompositionContextHelper::
                          CreateContext(g_compositor));

              auto interop = g_compositor.as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
              winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{nullptr};
              winrt::check_hresult(interop->CreateDesktopWindowTarget(
                  hwnd,
                  false,
                  reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget **>(
                      winrt::put_abi(target))));
              m_target = target;

              auto root = g_compositor.CreateContainerVisual();
              root.RelativeSizeAdjustment({1.0f, 1.0f});
              root.Offset({0, 0, 0});
              m_target.Root(root);
              m_compRootView
                  .as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
                  .SetWindow(reinterpret_cast<uint64_t>(hwnd));
              m_compRootView
                  .as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
                  .InternalRootVisual(winrt::Microsoft::ReactNative::Composition::Experimental::
                                          SystemCompositionContextHelper::CreateVisual(root));
              m_compRootView.ScaleFactor(ScaleFactor(hwnd));
              winrt::Microsoft::ReactNative::LayoutConstraints contraints;
              contraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
              contraints.MaximumSize =
                  contraints.MinimumSize = {m_width / ScaleFactor(hwnd), m_height / ScaleFactor(hwnd)};
              m_compRootView.Arrange(contraints, {0, 0});
            }
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
          RunPlayground(SW_SHOW, false);
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
                            auto asyncInfo, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
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

  LRESULT TranslateMessage(HWND /*hwnd*/, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    if (!m_useLiftedComposition && m_compRootView) {
      return static_cast<LRESULT>(
          m_compRootView.as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
              .SendMessage(message, wparam, lparam));
    }
    return 0;
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

  static constexpr std::wstring_view g_bundleFiles[] = {
      LR"(Samples\rntester)",
      LR"(Samples\accessible)",
      LR"(Samples\callbackTest)",
      LR"(Samples\calculator)",
      LR"(Samples\click)",
      LR"(Samples\customViewManager)",
      LR"(Samples\control)",
      LR"(Samples\flexbox)",
      LR"(Samples\focusTest)",
      LR"(Samples\geosample)",
      LR"(Samples\image)",
      LR"(Samples\index)",
      LR"(Samples\nativeFabricComponent)",
      LR"(Samples\mouse)",
      LR"(Samples\scrollViewSnapSample)",
      LR"(Samples\simple)",
      LR"(Samples\text)",
      LR"(Samples\textinput)",
      LR"(Samples\ticTacToe)",
      LR"(Samples\view)",
      LR"(Samples\debugTest01)"};

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
        CheckDlgButton(hwnd, IDC_LIFTEDCOMPOSITION, boolToCheck(self->m_useLiftedComposition));
        CheckDlgButton(hwnd, IDC_FASTREFRESH, boolToCheck(self->InstanceSettings().UseFastRefresh()));
        CheckDlgButton(hwnd, IDC_DIRECTDEBUGGER, boolToCheck(self->InstanceSettings().UseDirectDebugger()));
        CheckDlgButton(hwnd, IDC_BREAKONNEXTLINE, boolToCheck(self->InstanceSettings().DebuggerBreakOnNextLine()));
        CheckDlgButton(hwnd, IDC_SIZETOCONTENT, boolToCheck(self->m_sizeToContent));

        auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
        SetWindowTextW(portEditControl, std::to_wstring(self->InstanceSettings().DebuggerPort()).c_str());
        SendMessageW(portEditControl, (UINT)EM_SETLIMITTEXT, (WPARAM)5, (LPARAM)0);

        auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Chakra"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Hermes"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("V8"));
        // Fabric only supports Hermes right now - So dont actually set JS engine override
        // SendMessageW(cmbEngines, CB_SETCURSEL, (WPARAM) static_cast<int32_t>(self->m_jsEngine), (LPARAM)0);

        return TRUE;
      }
      case WM_COMMAND: {
        switch (LOWORD(wparam)) {
          case IDOK: {
            auto self = GetFromWindow(GetParent(hwnd));
            self->m_useLiftedComposition = (IsDlgButtonChecked(hwnd, IDC_LIFTEDCOMPOSITION) == BST_CHECKED);
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

            // Fabric only supports Hermes right now - So dont actually set JS engine override
            // auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
            // int itemIndex = (int)SendMessageW(cmbEngines, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            // self->m_jsEngine = static_cast<Microsoft::ReactNative::JSIEngine>(itemIndex);
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
  if (windowData) {
    auto result = WindowData::GetFromWindow(hwnd)->TranslateMessage(hwnd, message, wparam, lparam);
    if (result)
      return result;
  }

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
        async.Completed([host = data->m_host](auto asyncInfo, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
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
    case WM_GETOBJECT: {
      if (!windowData->m_useLiftedComposition && lparam == UiaRootObjectId) {
        if (windowData == nullptr || !windowData->m_compRootView)
          break;

        auto rootView = windowData->m_compRootView;
        winrt::com_ptr<IRawElementProviderSimple> spReps;
        if (!rootView.GetUiaProvider().try_as(spReps)) {
          break;
        }
        LRESULT lResult = UiaReturnRawElementProvider(hwnd, wparam, lparam, spReps.get());
        return lResult;
      }
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

int RunPlayground(int showCmd, bool /*useWebDebugger*/) {
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

  // Destroy all Composition objects
  g_compositor.Close();
  g_compositor = nullptr;

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

  DispatcherQueueOptions options{
      sizeof(DispatcherQueueOptions), /* dwSize */
      DQTYPE_THREAD_CURRENT, /* threadType */
      DQTAT_COM_ASTA /* apartmentType */
  };

  // Need to have a Dispatcher on the current thread to be able to create a Compositor
  winrt::check_hresult(CreateDispatcherQueueController(
      options,
      reinterpret_cast<ABI::Windows::System::IDispatcherQueueController **>(
          winrt::put_abi(g_dispatcherQueueController))));
  g_compositor = winrt::Windows::UI::Composition::Compositor();

  // Create a Lifted (WinAppSDK) DispatcherQueue for this thread.  This is needed for
  // Microsoft.UI.Composition, Content, and Input APIs.
  g_liftedDispatcherQueueController =
      winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();
  g_liftedCompositor = winrt::Microsoft::UI::Composition::Compositor();

// We only want to init XAML if we are using XAML islands
#ifdef USE_EXPERIMENTAL_WINUI3
  // Island-support: Create our custom Xaml App object. This is needed to properly use the controls and metadata
  // in Microsoft.ui.xaml.controls.dll.
  auto playgroundApp{winrt::make<winrt::Playground::implementation::App>()};
#endif

  return RunPlayground(showCmd, false);
}
