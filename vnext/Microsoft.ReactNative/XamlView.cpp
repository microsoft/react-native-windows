// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlView.h"
#include <UI.Composition.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Media.h>
#include "Utils/Helpers.h"

namespace Microsoft::ReactNative {

xaml::DependencyProperty ReactTagProperty() noexcept {
  static xaml::DependencyProperty s_tagProperty = xaml::DependencyProperty::RegisterAttached(
      L"ReactTag",
      winrt::xaml_typename<int64_t>(),
      winrt::xaml_typename<XamlView>(),
      xaml::PropertyMetadata(winrt::box_value(InvalidTag)));

  return s_tagProperty;
}

xaml::XamlRoot TryGetXamlRoot(const XamlView &view) {
  xaml::XamlRoot root{nullptr};
  if (auto uielement = view.try_as<xaml::UIElement>()) {
    root = uielement.XamlRoot();
  } else if (auto textelement = view.try_as<xaml::Documents::TextElement>()) {
    root = textelement.XamlRoot();
  } else if (auto flyout = view.try_as<xaml::Controls::Primitives::FlyoutBase>()) {
    root = flyout.XamlRoot();
  }
  return root;
}

thread_local comp::Compositor tlsCompositor{nullptr};

void SetCompositor(const comp::Compositor &compositor) {
  winrt::com_ptr<IUnknown> rawCompositor(winrt::get_abi(compositor), winrt::take_ownership_from_abi);
  if (tlsCompositor != nullptr) {
    assert(tlsCompositor == compositor);
  } else {
    tlsCompositor = compositor;
  }
}

comp::Compositor GetCompositor(const XamlView &view) {
  if (auto window = xaml::Window::Current()) {
    return window.Compositor();
  }
#ifndef USE_WINUI3
  assert(tlsCompositor != nullptr);
  return tlsCompositor;
#else
  return tlsCompositor ? tlsCompositor : xaml::Media::CompositionTarget::GetCompositorForCurrentThread();
#endif
}

} // namespace Microsoft::ReactNative
