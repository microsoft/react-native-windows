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
      winrt::Microsoft::ReactNative::IReactContext reactContext,
      winrt::Microsoft::ReactNative::Composition::ICompositionContext compContext)
      : m_compContext(compContext), m_reactContext(reactContext) {}

  ~CustomComponent() {
    m_xamlIsland.Close();
    m_siteBridge.Close();
    m_contentIsland.Close();
  }

  void UpdateProps(winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
    auto customProps = props.as<CustomProps>();
  }

  void UpdateLayoutMetrics(winrt::Microsoft::ReactNative::Composition::LayoutMetrics layoutMetrics) noexcept {
    m_layoutMetrics = layoutMetrics;
    m_visual.Size(
        {m_layoutMetrics.Frame.Width * m_layoutMetrics.PointScaleFactor,
         m_layoutMetrics.Frame.Height * m_layoutMetrics.PointScaleFactor});
    auto site = m_siteBridge.Site();
    auto siteWindow = site.Environment();
    auto displayScale = siteWindow.DisplayScale();

    site.ParentScale(displayScale);
    site.ActualSize(
        {m_layoutMetrics.Frame.Width /* * m_layoutMetrics.PointScaleFactor*/,
         m_layoutMetrics.Frame.Height /* * m_layoutMetrics.PointScaleFactor*/});
    site.ClientSize(winrt::Windows::Graphics::SizeInt32{
        static_cast<int32_t>(m_layoutMetrics.Frame.Width * m_layoutMetrics.PointScaleFactor),
        static_cast<int32_t>(m_layoutMetrics.Frame.Height * m_layoutMetrics.PointScaleFactor)});
  }

  static inline winrt::Microsoft::ReactNative::LayoutConstraints maxContraints{
      {0, 0} /*min*/,
      {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()} /* max*/,
      winrt::Microsoft::ReactNative::LayoutDirection::Undefined};

  struct MyStateData : winrt::implements<MyStateData, winrt::IInspectable> {
    MyStateData(winrt::Microsoft::ReactNative::LayoutConstraints c, winrt::Windows::Foundation::Size ds)
        : constraints(c), desiredSize(ds) {}

    winrt::Microsoft::ReactNative::LayoutConstraints constraints;
    winrt::Windows::Foundation::Size desiredSize;
  };

  winrt::Microsoft::UI::Xaml::UIElement CreateExpanderContent() {
    auto yogaXamlPanel = winrt::make<winrt::PlaygroundApp::implementation::YogaXamlPanel>(
        [&](winrt::Windows::Foundation::Size desiredSize) {
          if (m_state) {
            auto state = winrt::get_self<MyStateData>(m_state.Data());
            if (desiredSize != state->desiredSize) {
              m_state.UpdateStateWithMutation([desiredSize](winrt::Windows::Foundation::IInspectable data) {
                auto oldData = winrt::get_self<MyStateData>(data);
                return winrt::make<MyStateData>(oldData->constraints, desiredSize);
              });
            }
          }
        },
        [&]() -> const winrt::Microsoft::ReactNative::LayoutConstraints & {
          return maxContraints;
          /*
          if (!m_state)
          {
              return maxContraints;
          }
            auto state = winrt::get_self<MyStateData>(m_state.Data());
            return state->constraints;
          */
        });

    yogaXamlPanel.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush({255, 124, 124, 155}));
    yogaXamlPanel.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Stretch);
    yogaXamlPanel.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);

    auto expander = winrt::Microsoft::UI::Xaml::Controls::Expander();

    // expander.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Top);
    // expander.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Left);

    auto button1 = winrt::Microsoft::UI::Xaml::Controls::Button();
    button1.Content(winrt::box_value(L"This is a Xaml Button in Xaml Expander Header"));
    m_clickRevoker = button1.Click(
        winrt::auto_revoke,
        [wkExpander = winrt::weak_ref(expander)](
            winrt::Windows::Foundation::IInspectable const &sender,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const &e) noexcept {
          if (auto expander = wkExpander.get()) {
            sender.as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content(
                winrt::box_value(L"String with \nnew lines\nand more"));

            auto desiredSize = expander.DesiredSize();
          }
        });
    /*
    m_loadedRevoker = expander.Loaded(
        winrt::auto_revoke,
        [this, wkExpander = winrt::weak_ref(expander)](
            winrt::Windows::Foundation::IInspectable const &sender,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const &e) noexcept {
          if (auto expander = wkExpander.get()) {
            auto state = winrt::get_self<MyStateData>(m_state.Data());

            expander.Measure(state->constraints.MaximumSize);
            auto desiredSize = expander.DesiredSize();
            if (desiredSize != state->desiredSize) {
              m_state.UpdateState(winrt::make<MyStateData>(state->constraints, desiredSize));
            }
          }
        });
        */
    expander.Header(button1);

    auto button2 = winrt::Microsoft::UI::Xaml::Controls::Button();
    auto textblock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    textblock.Text(L"This is a Xaml TextBlock within the Expanders Content");
    textblock.TextTrimming(winrt::Microsoft::UI::Xaml::TextTrimming::CharacterEllipsis);
    // textblock.TextWrapping(winrt::Microsoft::UI::Xaml::TextWrapping::Wrap);
    button2.Content(winrt::box_value(L"This is a Xaml Button in the Expander's content"));
    button2.Margin({5, 5, 5, 5});

    // expander.Content(button2);
    expander.Content(textblock);

    yogaXamlPanel.Children().Append(expander);

    return yogaXamlPanel;
  }

  winrt::Microsoft::UI::Xaml::Controls::StackPanel CreateXamlContent() {
    // TextBox tries to use an HWND currently.. so doesn't work while hosted in a Visual
    // auto textBox = winrt::Microsoft::UI::Xaml::Controls::TextBox();
    // textBox.Text(L"Text Edit");

    auto button1 = winrt::Microsoft::UI::Xaml::Controls::Button();
    button1.Content(winrt::box_value(L"Test Xaml Button 1"));

    auto button2 = winrt::Microsoft::UI::Xaml::Controls::Button();
    button2.Content(winrt::box_value(L"Test Xaml Button 2"));

    auto toggle = winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch();
    toggle.Header(winrt::box_value(L"This is a Xaml ToggleSwitch"));

    auto text = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    text.Text(L"This is a Xaml TextBlock with a long string to test ellipisification.");
    text.TextTrimming(winrt::Microsoft::UI::Xaml::TextTrimming::CharacterEllipsis);

    auto stackPanel = winrt::Microsoft::UI::Xaml::Controls::StackPanel();
    stackPanel.Spacing(10);
    stackPanel.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromUniformLength(5.0));

    // stackPanel.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush({ 255,255,0,0 } /* red */));
    stackPanel.MinHeight(50);
    stackPanel.MinWidth(50);
    // stackPanel.Children().Append(textBox);
    stackPanel.Children().Append(button1);
    stackPanel.Children().Append(button2);
    stackPanel.Children().Append(toggle);
    stackPanel.Children().Append(text);

    return stackPanel;
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept {
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    // m_xamlIsland.Content(CreateXamlContent());
    m_xamlIsland.Content(CreateExpanderContent());

    m_contentIsland = m_xamlIsland.ContentIsland();
    m_contentIsland.RequestSize({100, 100});

    auto hwnd = reinterpret_cast<HWND>(
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(m_reactContext.Properties()));

    m_visual = m_compContext.CreateSpriteVisual();
    m_visual.Brush(m_compContext.CreateColorBrush({255, 255, 0, 255}));
    auto parentSystemVisual =
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerVisual(m_visual)
            .as<winrt::Windows::UI::Composition::ContainerVisual>();

    m_siteBridge = winrt::Microsoft::UI::Content::SystemVisualSiteBridge::Create(
        m_contentIsland.Compositor(), parentSystemVisual, winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));
    m_siteBridge.Connect(m_contentIsland);

    auto rootXamlVisualSize = m_contentIsland.Root().Size();

    return m_visual;
  }

  void UpdateState(winrt::Microsoft::ReactNative::IComponentState state) noexcept {
    bool newState = !!m_state;

    m_state = state;

    auto statedata = winrt::get_self<MyStateData>(m_state.Data());
    if (newState || m_layoutConstraints != statedata->constraints) {
      m_layoutConstraints = statedata->constraints;
      // m_xamlIsland.Content().Measure(m_layoutConstraints.MaximumSize);
      m_xamlIsland.Content().InvalidateMeasure();
    }

    // auto desiredSize = m_xamlIsland.Content().DesiredSize();
    // if (desiredSize != statedata->desiredSize)
    //{
    //     m_state.UpdateStateWithMutation([desiredSize](winrt::Windows::Foundation::IInspectable data)
    //     {
    //         auto oldData = winrt::get_self<MyStateData>(data);
    //         return winrt::make<MyStateData>(oldData->constraints, desiredSize);
    //     });
    // }
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
                return winrt::make<CustomComponent>(reactContext, context);
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

                // return {std::min(layoutContraints.MaximumSize.Width,currentState->desiredSize.width,
                // layoutContraints.MaximumSize.Height,currentState->desiredSize.height)}

                return currentState ? currentState->desiredSize : winrt::Windows::Foundation::Size{0, 0};
              });

          builder.SetLayoutHandler([](winrt::Microsoft::ReactNative::ShadowNode shadowNode,
                                      winrt::Microsoft::ReactNative::LayoutContext layoutContext) noexcept {
            auto currentState = winrt::get_self<MyStateData>(shadowNode.StateData());

            auto constraints = winrt::unbox_value_or<winrt::Microsoft::ReactNative::LayoutConstraints>(
                shadowNode.Tag(), maxContraints);
            if (currentState && constraints == currentState->constraints) {
              return;
            }

            // Now we are about to mutate the Shadow Node.
            shadowNode.EnsureUnsealed();
            /*
            shadowNode.StateData(winrt::make<MyStateData>(
                constraints, currentState ? currentState->desiredSize : winrt::Windows::Foundation::Size{0, 0}));
 */
            shadowNode.as<winrt::Microsoft::ReactNative::YogaLayoutableShadowNode>().Layout(layoutContext);
          });

          builder.SetInitialStateDataFactory([](const winrt::Microsoft::ReactNative::ViewProps & /*props*/) {
            return winrt::make<MyStateData>(maxContraints, winrt::Windows::Foundation::Size{0, 0});
          });
        });
  }

 private:
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Composition::SpotLight m_spotlight{nullptr};
#endif

  winrt::Microsoft::ReactNative::LayoutConstraints m_layoutConstraints;
  winrt::Microsoft::UI::Xaml::Controls::Button::Click_revoker m_clickRevoker;
  winrt::Microsoft::UI::Xaml::Controls::Expander::Loaded_revoker m_loadedRevoker;
  winrt::Microsoft::ReactNative::IComponentState m_state;
  winrt::Microsoft::ReactNative::Composition::LayoutMetrics m_layoutMetrics;
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
