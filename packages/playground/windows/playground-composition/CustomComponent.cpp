#include "pch.h"

#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.interop.h>
#endif

#include "App.CustomComponent.g.h"
#include <NativeModules.h>
#include "YogaXamlPanel.h"

namespace winrt::PlaygroundApp::implementation {

/*
 * Custom Properties can be passed from JS to this native component
 * This struct will eventually be codegen'd from the JS spec file
 */
REACT_STRUCT(CustomXamlComponentProps)
struct CustomXamlComponentProps
    : winrt::implements<CustomXamlComponentProps, winrt::Microsoft::ReactNative::IComponentProps> {
  CustomXamlComponentProps(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(label);
  winrt::hstring label;
  winrt::Microsoft::ReactNative::ViewProps m_props;
};

// Custom state used when using native layout to store the desired size of the Xaml control for use during RN
// layout on a background thread
struct CustomXamlComponentStateData : winrt::implements<CustomXamlComponentStateData, winrt::IInspectable> {
  CustomXamlComponentStateData(winrt::Windows::Foundation::Size ds) : desiredSize(ds) {}
  winrt::Windows::Foundation::Size desiredSize;
};

struct CustomComponent : CustomComponentT<CustomComponent> {
  CustomComponent(
      bool nativeLayout,
      const winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs &args)
      : base_type(args), m_nativeLayout(nativeLayout) {}

  ~CustomComponent() {
#ifdef USE_EXPERIMENTAL_WINUI3
    m_xamlIsland.Close();
    m_siteBridge.Close();
    // Hit a crash when calling m_contentIsland.Close?
    // m_contentIsland.Close();
#endif
  }

  void HandleCommand(winrt::hstring commandName, const winrt::Microsoft::ReactNative::IJSValueReader &args) {
    // Custom commands would be implemented here
    base_type::HandleCommand(commandName, args);
  }

  void UpdateProps(
      const winrt::Microsoft::ReactNative::IComponentProps &props,
      const winrt::Microsoft::ReactNative::IComponentProps &oldProps) {
    auto myProps = props.as<CustomXamlComponentProps>();

    m_buttonLabelTextBlock.Text(myProps->label);
  }
  void UpdateState(const winrt::Microsoft::ReactNative::IComponentState &state) {
    m_state = state;
  }

  void UpdateLayoutMetrics(
      const winrt::Microsoft::ReactNative::LayoutMetrics &layoutMetrics,
      const winrt::Microsoft::ReactNative::LayoutMetrics &oldLayoutMetrics) {
    base_type::UpdateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
#ifdef USE_EXPERIMENTAL_WINUI3
    auto site = m_siteBridge.Site();
    auto siteWindow = site.Environment();
    auto displayScale = siteWindow.DisplayScale();

    site.ParentScale(displayScale);
    site.ActualSize({layoutMetrics.Frame.Width, layoutMetrics.Frame.Height});
    site.ClientSize(winrt::Windows::Graphics::SizeInt32{
        static_cast<int32_t>(layoutMetrics.Frame.Width * layoutMetrics.PointScaleFactor),
        static_cast<int32_t>(layoutMetrics.Frame.Height * layoutMetrics.PointScaleFactor)});
#endif
  }

  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) {
    base_type::FinalizeUpdates(updateMask);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3

    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_xamlIsland.Content(CreateXamlButtonContent());

    m_contentIsland = m_xamlIsland.ContentIsland();
#endif

    m_visual = CompositionContext().CreateSpriteVisual();
    // m_visual.Brush(CompositionContext().CreateColorBrush({255, 255, 0, 255}));
#ifdef USE_EXPERIMENTAL_WINUI3

    auto parentSystemVisual =
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerVisual(m_visual)
            .as<winrt::Windows::UI::Composition::ContainerVisual>();

    auto hwnd = reinterpret_cast<HWND>(
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(ReactContext().Properties()));

    m_siteBridge = winrt::Microsoft::UI::Content::SystemVisualSiteBridge::Create(
        m_contentIsland.Compositor(), parentSystemVisual, winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));
    m_siteBridge.Connect(m_contentIsland);

    auto rootXamlVisualSize = m_contentIsland.Root().Size();
#endif

    return m_visual;
  }

  winrt::Microsoft::UI::Xaml::UIElement CreateXamlButtonContent() {
    m_buttonLabelTextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    m_buttonLabelTextBlock.Text(L"This is a Xaml Button set to ellipisify on truncation");
    m_buttonLabelTextBlock.TextTrimming(winrt::Microsoft::UI::Xaml::TextTrimming::CharacterEllipsis);

    auto button = winrt::Microsoft::UI::Xaml::Controls::Button();
    button.Margin({3, 3, 3, 3});
    button.Content(m_buttonLabelTextBlock);

    // If we are using native layout then wrap the element in a YogaXamlPanel which reports any changes to desired size
    // of the XAML element.
    if (!m_nativeLayout) {
      return button;
    }

    auto yogaXamlPanel = winrt::make<winrt::PlaygroundApp::implementation::YogaXamlPanel>(
        [&](winrt::Windows::Foundation::Size desiredSize) {
          if (m_state) {
            auto state = winrt::get_self<CustomXamlComponentStateData>(m_state.Data());
            if (desiredSize != state->desiredSize) {
              m_state.UpdateStateWithMutation([desiredSize](winrt::Windows::Foundation::IInspectable data) {
                auto oldData = winrt::get_self<CustomXamlComponentStateData>(data);
                return winrt::make<CustomXamlComponentStateData>(desiredSize);
              });
            }
          }
        });

    // yogaXamlPanel.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush({255, 124, 124, 155}));
    yogaXamlPanel.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Stretch);
    yogaXamlPanel.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);

    yogaXamlPanel.Children().Append(button);

    return yogaXamlPanel;
  }

 private:
  const bool m_nativeLayout;
  winrt::Microsoft::UI::Xaml::Controls::TextBlock m_buttonLabelTextBlock{nullptr};
  winrt::Microsoft::ReactNative::IComponentState m_state;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
#ifdef USE_EXPERIMENTAL_WINUI3
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Content::ContentIsland m_contentIsland{nullptr};
  winrt::Microsoft::UI::Content::SystemVisualSiteBridge m_siteBridge{nullptr};
#endif
};

static void RegisterViewComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"CustomXamlComponentWithNativeLayout",
      [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props) noexcept {
          return winrt::make<CustomXamlComponentProps>(props);
        });
        auto compBuilder =
            builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
        compBuilder.SetCreateViewComponentView(
            [](const winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs &args) noexcept {
              return winrt::make<CustomComponent>(true, args);
            });
        builder.SetMeasureContentHandler(
            [](winrt::Microsoft::ReactNative::ShadowNode shadowNode,
               winrt::Microsoft::ReactNative::LayoutContext layoutContext,
               winrt::Microsoft::ReactNative::LayoutConstraints layoutContraints) noexcept {
              shadowNode.Tag(winrt::box_value(layoutContraints));

              auto currentState = winrt::get_self<CustomXamlComponentStateData>(shadowNode.StateData());

              if (currentState) {
                // Snap up to the nearest whole pixel to avoid pixel snapping truncations
                auto size = winrt::Windows::Foundation::Size{
                    std::ceil(currentState->desiredSize.Width * layoutContext.PointScaleFactor()) /
                            layoutContext.PointScaleFactor() +
                        1.0f,
                    std::ceil(currentState->desiredSize.Height * layoutContext.PointScaleFactor()) /
                            layoutContext.PointScaleFactor() +
                        1.0f,
                };
                return size;
              }

              return winrt::Windows::Foundation::Size{0, 0};
            });
        builder.SetInitialStateDataFactory([](const winrt::Microsoft::ReactNative::IComponentProps & /*props*/) {
          return winrt::make<CustomXamlComponentStateData>(winrt::Windows::Foundation::Size{0, 0});
        });
      });

  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"CustomXamlComponentWithYogaLayout",
      [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props) noexcept {
          return winrt::make<CustomXamlComponentProps>(props);
        });
        auto compBuilder =
            builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
        compBuilder.SetCreateViewComponentView(
            [](const winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs &args) noexcept {
              return winrt::make<CustomComponent>(false, args);
            });
      });
}
} // namespace winrt::PlaygroundApp::implementation

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  winrt::PlaygroundApp::implementation::RegisterViewComponent(packageBuilder);
}
