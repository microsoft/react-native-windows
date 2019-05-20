// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>
#include "ReactControl.h"

#include <ReactUWP/InstanceFactory.h>
#include <CxxMessageQueue.h>
#include "unicode.h"
#include <Utils/ValueUtils.h>

#include <INativeUIManager.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react {
namespace uwp {

ReactControl::~ReactControl()
{
  if (m_reactInstance != nullptr)
  {
    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
  }
}

std::shared_ptr<IReactInstance> ReactControl::GetReactInstance() const noexcept
{
  return m_reactInstance;
}

void ReactControl::HandleInstanceError()
{
  auto weakThis = weak_from_this();
  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([weakThis]()
    {
      if (auto This = weakThis.lock())
      {
        This->HandleInstanceErrorOnUIThread();
      }
    });
}

void ReactControl::HandleInstanceErrorOnUIThread()
{
  if (m_reactInstance->IsInError())
  {
    auto xamlRootGrid(m_xamlRootView.as<winrt::Grid>());

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    // Create Grid & TextBlock to hold error text
    if (m_errorTextBlock == nullptr)
    {
      m_errorTextBlock = winrt::TextBlock();
      m_redBoxGrid = winrt::Grid();
      m_redBoxGrid.Background(winrt::SolidColorBrush(winrt::Colors::Crimson()));
      m_redBoxGrid.Children().Append(m_errorTextBlock);
    }

    // Add red box grid to root view
    xamlRootGrid.Children().Append(m_redBoxGrid);

    // Place error message into TextBlock
    std::wstring wstrErrorMessage(L"ERROR: Instance failed to start.\n\n");
    wstrErrorMessage += facebook::react::unicode::utf8ToUtf16(m_reactInstance->LastErrorMessage()).c_str();
    m_errorTextBlock.Text(wstrErrorMessage);

    // Format TextBlock
    m_errorTextBlock.TextAlignment(winrt::TextAlignment::Center);
    m_errorTextBlock.TextWrapping(winrt::TextWrapping::Wrap);
    m_errorTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
    m_errorTextBlock.Foreground(winrt::SolidColorBrush(winrt::Colors::White()));
    winrt::Thickness margin = { 10.0f, 10.0f, 10.0f, 10.0f };
    m_errorTextBlock.Margin(margin);
  }
}

void ReactControl::AttachRoot() noexcept
{
  if (m_isAttached)
    return;

  if (!m_reactInstance)
    m_reactInstance = m_instanceCreator->getInstance();

  // Handle any errors that occurred during creation before we add our callback
  if (m_reactInstance->IsInError())
    HandleInstanceError();

  if (!m_touchEventHandler)
    m_touchEventHandler = std::make_shared<TouchEventHandler>(m_reactInstance);

  // Register callback from instance for live reload
  m_errorCallbackCookie = m_reactInstance->RegisterErrorCallback([this]()
  {
    HandleInstanceError();
  });

  // Register callback from instance for live reload
  m_liveReloadCallbackCookie = m_reactInstance->RegisterLiveReloadCallback([this]()
  {
    auto weakThis = weak_from_this();
    m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([weakThis]()
    {
        if (auto This = weakThis.lock())
        {
          This->Reload(true);
        }
    });
  });

  // Schedule initialization that must happen on the UI thread
  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this]() {
    m_touchEventHandler->AddTouchHandlers(m_xamlRootView);
    auto initialProps = m_initialProps;
    m_reactInstance->AttachMeasuredRootView(m_pParent, std::move(initialProps));
  });

  m_isAttached = true;
}

void ReactControl::DetachRoot() noexcept
{
  if (!m_isAttached)
    return;

  if (m_touchEventHandler != nullptr)
  {
    m_touchEventHandler->RemoveTouchHandlers();
  }

  if (m_reactInstance != nullptr)
  {
    m_reactInstance->DetachRootView(m_pParent);

    // If the redbox error UI is shown we need to remove it, otherwise let the natural teardown process do this
    if (m_reactInstance->IsInError())
    {
      auto grid(m_xamlRootView.as<winrt::Grid>());
      if (grid != nullptr)
        grid.Children().Clear();

      m_redBoxGrid = nullptr;
      m_errorTextBlock = nullptr;
    }
  }

  m_isAttached = false;
}

void ReactControl::DetachInstance()
{
  if (m_reactInstance != nullptr)
  {
    std::shared_ptr<IReactInstance> instance = m_reactInstance;

    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
    m_reactInstance.reset();

    // Keep instance alive by capturing and queuing an empty func.
    // This extends the lifetime of NativeModules which may have
    // pending calls in these queues.
    // TODO prevent or check if even more is queued while these drain.
    CreateWorkerMessageQueue()->runOnQueue([instance](){});
    instance->DefaultNativeMessageQueueThread()->runOnQueue([instance](){});

    // Clear members with a dependency on the reactInstance
    m_touchEventHandler.reset();
  }
}

void ReactControl::Reload(bool shouldRetireCurrentInstance)
{
  // DetachRoot the current view and detach it
  DetachRoot();

  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this, shouldRetireCurrentInstance]() {

    if (shouldRetireCurrentInstance && m_reactInstance != nullptr)
      m_instanceCreator->markAsNeedsReload();

    // Detach ourselves from the instance
    DetachInstance();

    // Restart the view which will re-create the instance and attach the root view
    AttachRoot();
  });
}

XamlView ReactControl::GetXamlView() const noexcept
{
  return m_xamlRootView;
}

void ReactControl::SetJSComponentName(std::string&& jsComponentName) noexcept
{
  m_jsComponentName = std::move(jsComponentName);

  if (m_reactInstance != nullptr)
    m_reactInstance->SetAsNeedsReload();
}

void ReactControl::SetInstanceCreator(const ReactInstanceCreator& instanceCreator) noexcept
{
  // TODO - Handle swapping this out after the control is running
  m_instanceCreator = instanceCreator;
}

void ReactControl::SetInitialProps(folly::dynamic&& initialProps) noexcept
{
  m_initialProps = initialProps;
}

std::string ReactControl::JSComponentName() const noexcept
{
  return m_jsComponentName;
}

int64_t ReactControl::GetActualHeight() const
{
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualHeight());
}

int64_t ReactControl::GetActualWidth() const
{
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualWidth());
}

}
}
