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

#include "YogaXamlPanel.h"

/*
 * Custom Properties can be passed from JS to this native component
 * This struct will eventually be codegen'd from the JS spec file
 */
struct CustomProps : winrt::implements<CustomProps, winrt::Microsoft::ReactNative::IComponentProps> {
  CustomProps(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    if (propName == L"label") {
      if (!value) {
        label.clear();
      } else {
        label = winrt::to_string(value.GetString());
      }
    }
  }

  std::string label;
  winrt::Microsoft::ReactNative::ViewProps m_props;
};

struct CustomComponent : winrt::implements<CustomComponent, winrt::IInspectable> {
  CustomComponent(
      bool nativeLayout,
      winrt::Microsoft::ReactNative::IReactContext reactContext,
      winrt::Microsoft::ReactNative::Composition::ICompositionContext compContext)
      : m_nativeLayout(nativeLayout), m_compContext(compContext), m_reactContext(reactContext) {}

  ~CustomComponent() {
    m_xamlIsland.Close();
    m_siteBridge.Close();
    // Hit a crash when calling m_contentIsland.Close?
    // m_contentIsland.Close();
  }

  void UpdateProps(winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
    auto customProps = props.as<CustomProps>();
  }

  // This is called with the results of layout from RN.  We use this to set the size of the ContentIsland.
  void UpdateLayoutMetrics(winrt::Microsoft::ReactNative::Composition::LayoutMetrics layoutMetrics) noexcept {
    m_visual.Size(
        {layoutMetrics.Frame.Width * layoutMetrics.PointScaleFactor,
         layoutMetrics.Frame.Height * layoutMetrics.PointScaleFactor});
    auto site = m_siteBridge.Site();
    auto siteWindow = site.Environment();
    auto displayScale = siteWindow.DisplayScale();

    site.ParentScale(displayScale);
    site.ActualSize({layoutMetrics.Frame.Width, layoutMetrics.Frame.Height});
    site.ClientSize(winrt::Windows::Graphics::SizeInt32{
        static_cast<int32_t>(layoutMetrics.Frame.Width * layoutMetrics.PointScaleFactor),
        static_cast<int32_t>(layoutMetrics.Frame.Height * layoutMetrics.PointScaleFactor)});
  }

  // Custom state used in when using native layout to store the desired size of the Xaml control for use during RN
  // layout on a background thread
  struct MyStateData : winrt::implements<MyStateData, winrt::IInspectable> {
    MyStateData(winrt::Windows::Foundation::Size ds) : desiredSize(ds) {}
    winrt::Windows::Foundation::Size desiredSize;
  };

  winrt::Microsoft::UI::Xaml::UIElement CreateXamlButtonContent() {
    auto text = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    text.Text(L"This is a Xaml Button set to ellipisify on truncation");
    text.TextTrimming(winrt::Microsoft::UI::Xaml::TextTrimming::CharacterEllipsis);

    auto button = winrt::Microsoft::UI::Xaml::Controls::Button();
    button.Margin({3, 3, 3, 3});
    button.Content(text);

    // If we are using native layout then wrap the element in a YogaXamlPanel which reports any changes to desired size
    // of the XAML element.
    if (!m_nativeLayout) {
      return button;
    }

    auto yogaXamlPanel = winrt::make<winrt::PlaygroundApp::implementation::YogaXamlPanel>(
        [&](winrt::Windows::Foundation::Size desiredSize) {
          if (m_state) {
            auto state = winrt::get_self<MyStateData>(m_state.Data());
            if (desiredSize != state->desiredSize) {
              m_state.UpdateStateWithMutation([desiredSize](winrt::Windows::Foundation::IInspectable data) {
                auto oldData = winrt::get_self<MyStateData>(data);
                return winrt::make<MyStateData>(desiredSize);
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

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept {
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_xamlIsland.Content(CreateXamlButtonContent());

    m_contentIsland = m_xamlIsland.ContentIsland();

    m_visual = m_compContext.CreateSpriteVisual();
    // m_visual.Brush(m_compContext.CreateColorBrush({255, 255, 0, 255}));
    auto parentSystemVisual =
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerVisual(m_visual)
            .as<winrt::Windows::UI::Composition::ContainerVisual>();

    auto hwnd = reinterpret_cast<HWND>(
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(m_reactContext.Properties()));

    m_siteBridge = winrt::Microsoft::UI::Content::SystemVisualSiteBridge::Create(
        m_contentIsland.Compositor(), parentSystemVisual, winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));
    m_siteBridge.Connect(m_contentIsland);

    auto rootXamlVisualSize = m_contentIsland.Root().Size();

    return m_visual;
  }

  void UpdateState(winrt::Microsoft::ReactNative::IComponentState state) noexcept {
    m_state = state;
  }

  static void RegisterViewComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
    packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
        L"MyCustomComponent", [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
          builder.SetCreateProps(
              [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<CustomProps>(props); });
          auto compBuilder =
              builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
          compBuilder.SetCreateView(
              [](winrt::Microsoft::ReactNative::IReactContext reactContext,
                 winrt::Microsoft::ReactNative::Composition::ICompositionContext context) noexcept {
                return winrt::make<CustomComponent>(true, reactContext, context);
              });
          compBuilder.SetPropsUpdater([](winrt::Windows::Foundation::IInspectable componentHandle,
                                         winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
            componentHandle.as<CustomComponent>()->UpdateProps(props);
          });
          compBuilder.SetLayoutMetricsUpdater(
              [](winrt::Windows::Foundation::IInspectable componentHandle,
                 winrt::Microsoft::ReactNative::Composition::LayoutMetrics metrics) noexcept {
                componentHandle.as<CustomComponent>()->UpdateLayoutMetrics(metrics);
              });
          compBuilder.SetVisualCreator([](winrt::Windows::Foundation::IInspectable componentHandle) noexcept {
            return componentHandle.as<CustomComponent>()->CreateVisual();
          });
          builder.SetStateUpdater([](winrt::Windows::Foundation::IInspectable componentHandle,
                                     winrt::Microsoft::ReactNative::IComponentState state) noexcept {
            return componentHandle.as<CustomComponent>()->UpdateState(state);
          });

          builder.SetMeasureContentHandler(
              [](winrt::Microsoft::ReactNative::ShadowNode shadowNode,
                 winrt::Microsoft::ReactNative::LayoutContext layoutContext,
                 winrt::Microsoft::ReactNative::LayoutConstraints layoutContraints) noexcept {
                shadowNode.Tag(winrt::box_value(layoutContraints));

                auto currentState = winrt::get_self<MyStateData>(shadowNode.StateData());

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
            return winrt::make<MyStateData>(winrt::Windows::Foundation::Size{0, 0});
          });
        });

    packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
        L"MyCustomComponentYoga",
        [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
          builder.SetCreateProps(
              [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<CustomProps>(props); });
          auto compBuilder =
              builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
          compBuilder.SetCreateView(
              [](winrt::Microsoft::ReactNative::IReactContext reactContext,
                 winrt::Microsoft::ReactNative::Composition::ICompositionContext context) noexcept {
                return winrt::make<CustomComponent>(false, reactContext, context);
              });
          compBuilder.SetPropsUpdater([](winrt::Windows::Foundation::IInspectable componentHandle,
                                         winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
            componentHandle.as<CustomComponent>()->UpdateProps(props);
          });
          compBuilder.SetLayoutMetricsUpdater(
              [](winrt::Windows::Foundation::IInspectable componentHandle,
                 winrt::Microsoft::ReactNative::Composition::LayoutMetrics metrics) noexcept {
                componentHandle.as<CustomComponent>()->UpdateLayoutMetrics(metrics);
              });
          compBuilder.SetVisualCreator([](winrt::Windows::Foundation::IInspectable componentHandle) noexcept {
            return componentHandle.as<CustomComponent>()->CreateVisual();
          });
        });
  }

 private:
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Composition::SpotLight m_spotlight{nullptr};
#endif

  bool m_nativeLayout;
  winrt::Microsoft::ReactNative::IComponentState m_state;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
  winrt::Microsoft::ReactNative::IReactContext m_reactContext;
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Content::ContentIsland m_contentIsland{nullptr};
  winrt::Microsoft::UI::Content::SystemVisualSiteBridge m_siteBridge{nullptr};
};

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  CustomComponent::RegisterViewComponent(packageBuilder);
}
