#include "pch.h"

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>

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

// Should be codegen'd
REACT_STRUCT(OnMyEvent)
struct OnMyEvent {
  REACT_FIELD(value)
  bool value;

  REACT_FIELD(target)
  int32_t target;
};

// Should be codegen'd
struct CustomXamlComponentEventEmitter {
  CustomXamlComponentEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  void onMyEvent(OnMyEvent &value) const {
    m_eventEmitter.DispatchEvent(L"MyEvent", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

struct CustomComponentUserData : winrt::implements<CustomComponentUserData, winrt::IInspectable> {
  void Initialize(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView,
      bool nativeLayout) {
    nativeLayout;
    islandView;
#ifdef USE_EXPERIMENTAL_WINUI3
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_xamlIsland.Content(CreateXamlButtonContent(nativeLayout));
    islandView.Connect(m_xamlIsland.ContentIsland());
#endif
  }

  void PropsChanged(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView & /*islandView*/,
      const winrt::Microsoft::ReactNative::IComponentProps &newProps,
      const winrt::Microsoft::ReactNative::IComponentProps & /*oldProps*/) {
    auto myProps = newProps.as<CustomXamlComponentProps>();
#ifdef USE_EXPERIMENTAL_WINUI3
    m_buttonLabelTextBlock.Text(myProps->label);
#endif
  }

  void FinalizeUpdates() noexcept {
    if (m_eventEmitter) {
      OnMyEvent args;
      args.value = false;
      m_eventEmitter->onMyEvent(args);
    }
  }

  winrt::Microsoft::UI::Xaml::UIElement CreateXamlButtonContent(bool nativeLayout) {
    m_buttonLabelTextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    m_buttonLabelTextBlock.Text(L"This is a Xaml Button set to ellipisify on truncation");
    m_buttonLabelTextBlock.TextTrimming(winrt::Microsoft::UI::Xaml::TextTrimming::CharacterEllipsis);

    auto button = winrt::Microsoft::UI::Xaml::Controls::Button();
    button.Margin({3, 3, 3, 3});
    button.Content(m_buttonLabelTextBlock);

    // If we are using native layout then wrap the element in a YogaXamlPanel which reports any changes to desired size
    // of the XAML element.
    if (!nativeLayout) {
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

  static void ConfigureBuilderForCustomComponent(
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder,
      bool nativeLayout) {
    builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props) noexcept {
      return winrt::make<CustomXamlComponentProps>(props);
    });

    builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &source,
                                        winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) {
      auto userData = source.UserData().as<CustomComponentUserData>();
      userData->FinalizeUpdates();
    });

    auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

    compBuilder.SetContentIslandComponentViewInitializer(
        [nativeLayout](
            const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
          auto userData = winrt::make_self<CustomComponentUserData>();
          userData->Initialize(islandView, nativeLayout);
          islandView.UserData(*userData);

#ifdef USE_EXPERIMENTAL_WINUI3
          islandView.Destroying([](const winrt::IInspectable &sender, const winrt::IInspectable &args) {
            auto senderIslandView = sender.as<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView>();
            auto userData = senderIslandView.UserData().as<CustomComponentUserData>();
            userData->m_xamlIsland.Close();
          });
#endif
        });

    builder.SetUpdateEventEmitterHandler([](const winrt::Microsoft::ReactNative::ComponentView &source,
                                            const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) {
      auto senderIslandView = source.as<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView>();
      auto userData = senderIslandView.UserData().as<CustomComponentUserData>();
      userData->m_eventEmitter = std::make_unique<CustomXamlComponentEventEmitter>(eventEmitter);
    });

    if (nativeLayout) {
      builder.SetMeasureContentHandler([](winrt::Microsoft::ReactNative::ShadowNode shadowNode,
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
      builder.SetInitialStateDataFactory([](const winrt::Microsoft::ReactNative::IComponentProps & /*props*/) noexcept {
        return winrt::make<CustomXamlComponentStateData>(winrt::Windows::Foundation::Size{0, 0});
      });
    }

    builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &source,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) {
      auto senderIslandView = source.as<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView>();
      auto userData = senderIslandView.UserData().as<CustomComponentUserData>();
      userData->PropsChanged(senderIslandView, newProps, oldProps);
    });

    builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &source,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) {
      auto senderIslandView = source.as<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView>();
      auto userData = senderIslandView.UserData().as<CustomComponentUserData>();
      userData->m_state = newState;
    });
  }

 private:
  winrt::Microsoft::UI::Xaml::Controls::TextBlock m_buttonLabelTextBlock{nullptr};
  winrt::Microsoft::ReactNative::IComponentState m_state;
  std::unique_ptr<CustomXamlComponentEventEmitter> m_eventEmitter{nullptr};
#ifdef USE_EXPERIMENTAL_WINUI3
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
#endif
};

static void RegisterViewComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"CustomXamlComponentWithNativeLayout",
      [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        CustomComponentUserData::ConfigureBuilderForCustomComponent(builder, true /*nativeLayout*/);
      });

  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"CustomXamlComponentWithYogaLayout",
      [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        CustomComponentUserData::ConfigureBuilderForCustomComponent(builder, false /*nativeLayout*/);
      });
}
} // namespace winrt::PlaygroundApp::implementation

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  winrt::PlaygroundApp::implementation::RegisterViewComponent(packageBuilder);
}
