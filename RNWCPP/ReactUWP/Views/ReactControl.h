// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <stdint.h>
#include <memory>
#include <set>

#include "IXamlRootView.h"
#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
}

namespace react {
namespace uwp {

class ReactControl
{
public:
  ReactControl(IXamlRootView* parent, XamlView rootView)
    : m_pParent(parent),
    m_xamlRootView(rootView),
    m_touchId(0)
  {
  }

  virtual ~ReactControl();

  XamlView GetXamlView() const noexcept;
  std::shared_ptr<IReactInstance> GetReactInstance() const noexcept;
  void SetJSComponentName(std::string&& mainComponentName) noexcept;
  void SetInstanceCreator(const ReactInstanceCreator& instanceCreator) noexcept;
  void SetInitialProps(folly::dynamic&& initialProps) noexcept;

  void AttachRoot() noexcept;
  void DetachRoot() noexcept;
  void DetachInstance();
  void Reload(bool shouldRetireCurrentInstance);

  void ResetView() {}
  virtual std::string JSComponentName() const noexcept;
  virtual int64_t GetActualHeight() const;
  virtual int64_t GetActualWidth() const;
  int64_t GetTag() const { return m_rootTag; }
  void SetTag(int64_t tag) { m_rootTag = tag; }

private:
  // Touch Event Handling
  void AddTouchHandlers();
  void OnPointerPressed(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  void OnPointerReleased(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  void OnPointerCanceled(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  void OnPointerCaptureLost(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  void OnPointerExited(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  void OnPointerMoved(const winrt::IInspectable&, const winrt::PointerRoutedEventArgs& args);
  winrt::event_revoker<winrt::IUIElement> m_pressedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_releasedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_canceledRevoker;
  winrt::event_revoker<winrt::IUIElement> m_captureLostRevoker;
  winrt::event_revoker<winrt::IUIElement> m_exitedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_movedRevoker;

  struct ReactPointer
  {
    int64_t target = 0;
    int64_t identifier = 0;
    uint32_t pointerId = 0;
    uint64_t timestamp = 0;
    winrt::Point positionRoot = { 0, 0 };
    winrt::Point positionView = { 0, 0 };
    winrt::Windows::Devices::Input::PointerDeviceType deviceType;
    float pressure = 0;
    bool isLeftButton = false;
    bool isRightButton = false;
    bool isMiddleButton = false;
    bool isBarrelButton = false;
    bool isHorizontalScrollWheel = false;
    bool isEraser = false;
    bool shiftKey = false;
    bool ctrlKey = false;
    bool altKey = false;
  };
  size_t AddReactPointer(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement);
  ReactPointer CreateReactPointer(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement);
  void UpdateReactPointer(ReactPointer& pointer, const winrt::PointerRoutedEventArgs& args, winrt::FrameworkElement sourceElement);
  void UpdatePointersInViews(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement);
  void SendPointerMove(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement);

  enum class TouchEventType
  {
    Start = 0,
    End,
    Move,
    Cancel,
    PointerEntered,
    PointerExited,
    PointerMove
  };
  void OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs& args);
  void DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex);
  const char* GetPointerDeviceTypeName(winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept;
  const char* GetTouchEventTypeName(TouchEventType eventType) noexcept;

  size_t IndexOfPointerWithId(uint32_t pointerId);
  static const size_t s_InvalidPointerId = (size_t)-1;
  folly::dynamic GetPointerJson(const ReactPointer& pointer, size_t targetOverride);
  std::vector<ReactPointer> m_pointers;
  std::unordered_map<uint32_t/*pointerId*/, std::set<int64_t>/*tags*/> m_pointersInViews;
  int64_t m_touchId;

  bool TagFromOriginalSource(const winrt::PointerRoutedEventArgs& args, int64_t* pTag, winrt::FrameworkElement* pSourceElement);
  std::set<int64_t> GetTagsAtPoint(const winrt::PointerRoutedEventArgs& e);

  void HandleInstanceError();

private:
  IXamlRootView* m_pParent;

  std::string m_jsComponentName;
  std::shared_ptr<facebook::react::NativeModuleProvider> m_moduleProvider;
  folly::dynamic m_initialProps;

  int64_t m_rootTag = -1;
  XamlView m_xamlRootView;
  ReactInstanceCreator m_instanceCreator;
  std::shared_ptr<IReactInstance> m_reactInstance;
  bool m_isAttached { false };
  LiveReloadCallbackCookie m_liveReloadCallbackCookie { 0 };
  ErrorCallbackCookie m_errorCallbackCookie { 0 };

  winrt::Grid m_redBoxGrid { nullptr };
  winrt::TextBlock m_errorTextBlock { nullptr };
};

}
}
