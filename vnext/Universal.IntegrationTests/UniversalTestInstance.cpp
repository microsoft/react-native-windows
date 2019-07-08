// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "UniversalTestInstance.h"

#include <Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.h>
#include <wrl.h>

namespace xaml = Windows::UI::Xaml;

using namespace react::uwp;

using std::shared_ptr;

namespace Microsoft::React::Test {

#pragma region UniversalTestInstance members

struct TestReactInstanceCreator : ::react::uwp::IReactInstanceCreator {
  TestReactInstanceCreator(
      std::shared_ptr<::react::uwp::IReactInstance> &pInstance) {
    m_instance = pInstance;
  }

  std::shared_ptr<::react::uwp::IReactInstance> getInstance() {
    return m_instance;
  }

  void markAsNeedsReload() {
    // Test instance isn't reloaded
  }

 private:
  std::shared_ptr<::react::uwp::IReactInstance> m_instance;
};

UniversalTestInstance::UniversalTestInstance(
    shared_ptr<IReactInstance> instance) noexcept
    : m_instance{std::move(instance)} {
  m_instanceCreator = std::make_shared<TestReactInstanceCreator>(instance);
}

shared_ptr<facebook::react::Instance> UniversalTestInstance::GetInnerInstance()
    const noexcept {
  return m_instanceCreator->getInstance()->GetInnerInstance();
}

void UniversalTestInstance::AttachMeasuredRootView(
    std::string &&appName) noexcept {
  // Instantiate root view.
  auto action =
      Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow
          ->Dispatcher->RunAsync(
              Windows::UI::Core::CoreDispatcherPriority::Normal,
              ref new Windows::UI::Core::DispatchedHandler([this]() {
                auto frame = xaml::Window::Current->Content;
                auto presenter =
                    xaml::Media::VisualTreeHelper::GetChild(frame, 0);
                auto page =
                    xaml::Media::VisualTreeHelper::GetChild(presenter, 0);
                auto mainGrid = static_cast<xaml::Controls::Grid ^>(
                    xaml::Media::VisualTreeHelper::GetChild(page, 0));

                xaml::IFrameworkElement ^ rootFrameworkElement = mainGrid;
                Microsoft::WRL::ComPtr<
                    ::ABI::Windows::UI::Xaml::IFrameworkElement>
                    spFrameworkElementABI = reinterpret_cast<
                        ABI::Windows::UI::Xaml::IFrameworkElement *>(
                        rootFrameworkElement);
                // Create C++/WinRT pointer from ABI pointer.
                ::react::uwp::XamlView xamlView = reinterpret_cast<
                    const winrt::Windows::UI::Xaml::FrameworkElement &>(
                    spFrameworkElementABI);

                m_rootView = ::react::uwp::CreateReactRootView(
                    xamlView, L"DummyTest", m_instanceCreator);

                m_instance->AttachMeasuredRootView(m_rootView.get(), {});
              }));
}

void UniversalTestInstance::DetachRootView() noexcept {
  m_instance->DetachRootView(m_rootView.get());
}

#pragma endregion

} // namespace Microsoft::React::Test
