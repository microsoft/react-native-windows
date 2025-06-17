#pragma once

#if !defined(RNW_NEW_ARCH)
static_assert(false, "This module requires RNW_NEW_ARCH to be defined.");
#endif

#include <NativeModules.h>

#include <JSValueComposition.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.Xaml.h>

#include <winrt/Microsoft.UI.Composition.h>

#include <winrt/Microsoft.UI.Xaml.Controls.h>

inline std::wstring ToWideString(const char *str) {
  int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
  if (len == 0) {
    return nullptr; // Conversion error
  }

  // Allocate a wstring with buffer size of len
  std::wstring result(len, L'\0');

  // Convert the std::string to BSTR
  MultiByteToWideChar(CP_UTF8, 0, str, -1, &result[0], len);

  return result;
}

namespace winrt::Microsoft::ReactNative::Xaml {

// A FabricXamlControl is a control that represents a generic Xaml element.
// The m_xamlTypeName field tells us what kind of Xaml element.
// A tree of FabricXamlControls must have a XamlHost as a root.
REACT_STRUCT(FabricXamlControlProps)
struct FabricXamlControlProps
    : winrt::implements<FabricXamlControlProps, winrt::Microsoft::ReactNative::IComponentProps> {
  FabricXamlControlProps(
      winrt::hstring name,
      winrt::Microsoft::ReactNative::ViewProps props,
      const winrt::Microsoft::ReactNative::IComponentProps &cloneFrom)
      : ViewProps(props), xamlTypeName(name) {
    if (cloneFrom) {
      auto cloneFromProps = cloneFrom.as<FabricXamlControlProps>();
      label = cloneFromProps->label;
      xamlTypeName = cloneFromProps->xamlTypeName;
    }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(label)
  std::string label;

  REACT_FIELD(text)
  std::string text;

  REACT_FIELD(content)
  // winrt::IInspectable content;
  winrt::Microsoft::ReactNative::JSValue content;

  REACT_FIELD(width)
  std::optional<float> width;

  REACT_FIELD(height)
  std::optional<float> height;

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;

  winrt::hstring xamlTypeName;
};

struct FabricXamlControlEventEmitter {
  FabricXamlControlEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template <typename TUserData>
struct BaseFabricXamlControl {
  virtual void UpdateProps(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::com_ptr<FabricXamlControlProps> &newProps,
      const winrt::com_ptr<FabricXamlControlProps> & /*oldProps*/) noexcept {
    m_props = newProps;
  }

  // UpdateLayoutMetrics will only be called if this method is overridden
  virtual void UpdateLayoutMetrics(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::LayoutMetrics & /*newLayoutMetrics*/,
      const winrt::Microsoft::ReactNative::LayoutMetrics & /*oldLayoutMetrics*/) noexcept {}

  // UpdateState will only be called if this method is overridden
  virtual void UpdateState(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::IComponentState & /*newState*/) noexcept {}

  virtual void UpdateEventEmitter(const std::shared_ptr<FabricXamlControlEventEmitter> &eventEmitter) noexcept {
    m_eventEmitter = eventEmitter;
  }

  // MountChildComponentView will only be called if this method is overridden
  virtual void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs & /*args*/) noexcept {}

  // UnmountChildComponentView will only be called if this method is overridden
  virtual void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs & /*args*/) noexcept {}

  // Initialize will only be called if this method is overridden
  virtual void Initialize(const winrt::Microsoft::ReactNative::ComponentView & /*view*/) noexcept {}

  // CreateVisual will only be called if this method is overridden
  virtual winrt::Microsoft::UI::Composition::Visual CreateVisual(
      const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    return view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Compositor().CreateSpriteVisual();
  }

  // FinalizeUpdate will only be called if this method is overridden
  virtual void FinalizeUpdate(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) noexcept {}

  const std::shared_ptr<FabricXamlControlEventEmitter> &EventEmitter() const {
    return m_eventEmitter;
  }
  const winrt::com_ptr<FabricXamlControlProps> &Props() const {
    return m_props;
  }

 private:
  winrt::com_ptr<FabricXamlControlProps> m_props;
  std::shared_ptr<FabricXamlControlEventEmitter> m_eventEmitter;
};

struct XamlControlComponentView : public winrt::implements<XamlControlComponentView, 
      winrt::Microsoft::ReactNative::Xaml::IXamlControl, winrt::IInspectable>,
                                  BaseFabricXamlControl<XamlControlComponentView> {
  XamlControlComponentView(winrt::hstring name) : m_xamlTypeName(name) {
    ::OutputDebugString(L"Microsoft::ReactNative::Xaml constructor\n");
    ::OutputDebugString(m_xamlTypeName.c_str());
    ::OutputDebugString(L"\n");
  }

  // IXamlControl
  winrt::Microsoft::UI::Xaml::UIElement GetXamlElement() {
    return GetXamlElementInternal();
  }

  // MountChildComponentView will only be called if this method is overridden
  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept override {
    // We're adding a child Xaml element to this Xaml element.
    auto child = args.Child();
    auto userData = child.UserData();
    //auto childXamlControl = userData.as<XamlControlComponentView>();
    auto childXamlControl = userData.as<winrt::Microsoft::ReactNative::Xaml::IXamlControl>();
    if (childXamlControl) {
      auto childXamlElement = childXamlControl.GetXamlElement();
      auto thisElement = GetXamlElement();
      AddXamlChild(thisElement, childXamlElement, args.Index());
    }
  }

  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept override {
    // We're adding a child Xaml element to this Xaml element.
    auto child = args.Child();
    auto userData = child.UserData();
    auto childXamlControl = userData.as<winrt::Microsoft::ReactNative::Xaml::IXamlControl>();
    if (childXamlControl) {
      auto thisElement = GetXamlElement();
      RemoveXamlChild(thisElement, args.Index());
    }
  }

  void UpdateProps(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::com_ptr<FabricXamlControlProps> &newProps,
      const winrt::com_ptr<FabricXamlControlProps> & /*oldProps*/) noexcept override {
    auto xamlControl = GetXamlElement();

    if (newProps != nullptr) {
      if (newProps->content.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto content = newProps->content.AsString();
        // const std::wstring url16 = Microsoft::Common::Unicode::Utf8ToUtf16(content);
        auto string16 = ToWideString(content.c_str());
        winrt::hstring contentHString(string16.c_str());
        xamlControl.SetValue(
            winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentProperty(), winrt::box_value(contentHString));
      } else if (newProps->content.Type() == winrt::Microsoft::ReactNative::JSValueType::Null) {
        // xamlControl.SetValue(winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentProperty,
        // winrt::hstring{L""});
      } else {
        // TODO: Better error handling here.
        ::OutputDebugString(L"UpdateProps: I don't recognize this type.\n");
      }

      if (newProps->width) {
        xamlControl.as<winrt::Microsoft::UI::Xaml::FrameworkElement>().Width(newProps->width.value());
      }
      if (newProps->height) {
        xamlControl.as<winrt::Microsoft::UI::Xaml::FrameworkElement>().Height(newProps->height.value());
      }
    }
  }

  void FinalizeUpdate(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept override {
    UNREFERENCED_PARAMETER(mask);
    view.Mounted([](const winrt::IInspectable &, const winrt::Microsoft::ReactNative::ComponentView &view) {
      UNREFERENCED_PARAMETER(view);
      // Do mounted things (?)
    });
    ::OutputDebugString(L"FinalizeUpdate\n");
  }

  static void AddXamlChild(
      winrt::Microsoft::UI::Xaml::UIElement parent,
      winrt::Microsoft::UI::Xaml::UIElement child,
      uint32_t index) {
    // TODO: Support all kinds of Xaml panels and elements that can accept children.
    auto panel = parent.as<winrt::Microsoft::UI::Xaml::Controls::Panel>();
    panel.Children().InsertAt(index, child);
  }

  static void RemoveXamlChild(winrt::Microsoft::UI::Xaml::UIElement parent, uint32_t index) {
    // TODO: Support all kinds of Xaml panels and elements that can accept children.
    auto panel = parent.as<winrt::Microsoft::UI::Xaml::Controls::Panel>();
    auto children = panel.Children();
    children.RemoveAt(index);
  }

  winrt::Microsoft::UI::Xaml::UIElement GetXamlElementInternal() {
    if (!m_xamlElement) {
      if (wcscmp(m_xamlTypeName.c_str(), L"FX_StackPanel") == 0) {
        m_xamlElement = winrt::Microsoft::UI::Xaml::Controls::StackPanel();
      } else if (wcscmp(m_xamlTypeName.c_str(), L"FX_Button") == 0) {
        m_xamlElement = winrt::Microsoft::UI::Xaml::Controls::Button();
      } else if (wcscmp(m_xamlTypeName.c_str(), L"FX_CalendarView") == 0) {
        m_xamlElement = winrt::Microsoft::UI::Xaml::Controls::CalendarView();
      } else {
        throw std::exception("Unknown XAML control type");
      }
    }
    return m_xamlElement;
  }

 private:
  winrt::Microsoft::UI::Xaml::UIElement m_xamlElement{nullptr};
  winrt::hstring m_xamlTypeName;
};

} // namespace winrt::Microsoft::ReactNative::Xaml

// #endif // #if defined(RNW_NEW_ARCH)