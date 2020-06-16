// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactRootView.h>

#include "ReactControl.h"

#include <IReactInstance.h>

#include <ReactUWP/InstanceFactory.h>

#include <CxxMessageQueue.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt {
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Input;
using namespace Windows::Foundation;
using namespace xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

ReactRootView::ReactRootView(XamlView rootView) {
  m_pimpl = std::make_shared<ReactControl>(this, rootView);
}

ReactRootView::~ReactRootView() {}

std::shared_ptr<IReactInstance> ReactRootView::GetReactInstance() const noexcept {
  return m_pimpl->GetReactInstance();
}

XamlView ReactRootView::GetXamlView() const noexcept {
  return m_pimpl->GetXamlView();
}

void ReactRootView::SetJSComponentName(std::string &&jsComponentName) noexcept {
  m_pimpl->SetJSComponentName(std::move(jsComponentName));
}

void ReactRootView::SetInstanceCreator(const ReactInstanceCreator &instanceCreator) noexcept {
  m_pimpl->SetInstanceCreator(instanceCreator);
}

void ReactRootView::SetInitialProps(folly::dynamic &&initialProps) noexcept {
  m_pimpl->SetInitialProps(std::move(initialProps));
}

void ReactRootView::AttachRoot() noexcept {
  m_pimpl->AttachRoot();
}

void ReactRootView::DetachRoot() noexcept {
  m_pimpl->DetachRoot();
}

std::shared_ptr<::react::uwp::IXamlReactControl> ReactRootView::GetXamlReactControl() const noexcept {
  return m_pimpl;
}

void ReactRootView::ResetView() {
  return m_pimpl->ResetView();
}

std::string ReactRootView::JSComponentName() const noexcept {
  return m_pimpl->JSComponentName();
}

int64_t ReactRootView::GetActualHeight() const {
  return m_pimpl->GetActualHeight();
}

int64_t ReactRootView::GetActualWidth() const {
  return m_pimpl->GetActualWidth();
}

int64_t ReactRootView::GetTag() const {
  return m_pimpl->GetTag();
}

void ReactRootView::SetTag(int64_t tag) {
  return m_pimpl->SetTag(tag);
}

} // namespace uwp
} // namespace react
