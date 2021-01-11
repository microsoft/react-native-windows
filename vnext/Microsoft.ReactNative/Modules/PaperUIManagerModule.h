// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <CxxMessageQueue.h>
#include <INativeUIManager.h>
#include <NativeModules.h>
#include <React.h>
#include <Views/ShadowNodeRegistry.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>

namespace Microsoft::ReactNative {

class UIManagerModule;
class IViewManager;
class NativeUIManager;

// Settings to configure the UIManager -- these should be pushed to the instance settings before the creation of the
// UIManager
struct UIManagerSettings {
  UIManagerSettings(
      const std::shared_ptr<facebook::react::MessageQueueThread> batchingUIMessageQueue,
      std::vector<std::unique_ptr<IViewManager>> &&viewManagers);
  UIManagerSettings(UIManagerSettings const &) = delete;
  const std::shared_ptr<facebook::react::MessageQueueThread> batchingUIMessageQueue;
  std::vector<std::unique_ptr<IViewManager>> viewManagers;
};

std::weak_ptr<NativeUIManager> GetNativeUIManager(const Mso::React::IReactContext &context);

REACT_MODULE(UIManager)
struct UIManager final {
  UIManager();
  ~UIManager();

  static void SetSettings(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      std::unique_ptr<UIManagerSettings> &&settings) noexcept;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider)
  void ConstantsViaConstantsProvider(winrt::Microsoft::ReactNative::ReactConstantProvider &constants) noexcept;

  REACT_SYNC_METHOD(getConstantsForViewManager)
  winrt::Microsoft::ReactNative::JSValueObject getConstantsForViewManager(std::string viewManagerName) noexcept;

  // Not part of the spec, but core polyfils this on the JS side.
  REACT_SYNC_METHOD(getViewManagerConfig)
  winrt::Microsoft::ReactNative::JSValueObject getViewManagerConfig(std::string viewManagerName) noexcept;

  REACT_SYNC_METHOD(getDefaultEventTypes)
  winrt::Microsoft::ReactNative::JSValueArray getDefaultEventTypes() noexcept;

  REACT_SYNC_METHOD(lazilyLoadView)
  winrt::Microsoft::ReactNative::JSValueObject lazilyLoadView(std::string name) noexcept;

  REACT_METHOD(createView)
  void createView(
      double reactTag, // How come these cannot be int64_t?
      std::string viewName,
      double rootTag,
      winrt::Microsoft::ReactNative::JSValueObject &&props) noexcept;

  REACT_METHOD(updateView)
  void updateView(double reactTag, std::string viewName, winrt::Microsoft::ReactNative::JSValueObject &&props) noexcept;

  REACT_METHOD(focus)
  void focus(double reactTag) noexcept;

  REACT_METHOD(blur)
  void blur(double reactTag) noexcept;

  REACT_METHOD(findSubviewIn)
  void findSubviewIn(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValueArray &&point,
      std::function<void(double nativeViewTag, double left, double top, double width, double height)> const
          &callback) noexcept;

  // The spec is incorrect in that it specifies a number for the command ID, but its actually a number or a string
  REACT_METHOD(dispatchViewManagerCommand)
  void dispatchViewManagerCommand(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValue &&commandID,
      winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) noexcept;

  REACT_METHOD(measure)
  void measure(
      double reactTag,
      std::function<void(double left, double top, double width, double height, double pageX, double pageY)> const
          &callback) noexcept;

  REACT_METHOD(measureInWindow)
  void measureInWindow(
      double reactTag,
      std::function<void(double x, double y, double width, double height)> const &callback) noexcept;

  REACT_METHOD(viewIsDescendantOf)
  void viewIsDescendantOf(
      double reactTag,
      double ancestorReactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) noexcept;

  REACT_METHOD(measureLayout)
  void measureLayout(
      double reactTag,
      double ancestorReactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback,
      std::function<void(double left, double top, double width, double height)> const &callback) noexcept;

  REACT_METHOD(measureLayoutRelativeToParent)
  void measureLayoutRelativeToParent(
      double reactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) noexcept;

  REACT_METHOD(setJSResponder)
  void setJSResponder(double reactTag, bool blockNativeResponder) noexcept;

  REACT_METHOD(clearJSResponder)
  void clearJSResponder() noexcept;

  REACT_METHOD(configureNextLayoutAnimation)
  void configureNextLayoutAnimation(
      winrt::Microsoft::ReactNative::JSValueObject &&config,
      std::function<void()> const &callback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback) noexcept;

  REACT_METHOD(removeSubviewsFromContainerWithID)
  void removeSubviewsFromContainerWithID(double containerID) noexcept;

  REACT_METHOD(replaceExistingNonRootView)
  void replaceExistingNonRootView(double reactTag, double newReactTag) noexcept;

  REACT_METHOD(removeRootView)
  void removeRootView(double reactTag) noexcept;

  REACT_METHOD(setChildren)
  void setChildren(double containerTag, winrt::Microsoft::ReactNative::JSValueArray &&reactTags) noexcept;

  REACT_METHOD(manageChildren)
  void manageChildren(
      double containerTag,
      winrt::Microsoft::ReactNative::JSValueArray &&moveFromIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&moveToIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&addChildReactTags,
      winrt::Microsoft::ReactNative::JSValueArray &&addAtIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&removeAtIndices) noexcept;

  REACT_METHOD(setLayoutAnimationEnabledExperimental)
  void setLayoutAnimationEnabledExperimental(bool enabled) noexcept;

  REACT_METHOD(sendAccessibilityEvent)
  void sendAccessibilityEvent(double reactTag, double eventType) noexcept;

  REACT_METHOD(showPopupMenu)
  void showPopupMenu(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValueArray &&items,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &error,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &success) noexcept;

  REACT_METHOD(dismissPopupMenu)
  void dismissPopupMenu() noexcept;

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> m_batchingUIMessageQueue;
  std::shared_ptr<UIManagerModule> m_module;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
