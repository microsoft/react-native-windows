// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlView.h"
#include <UI.Composition.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Documents.h>
#include "Utils/Helpers.h"

namespace Microsoft::ReactNative {

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

comp::Compositor GetCompositor(const XamlView &view) {
  if (auto window = xaml::Window::Current()) {
    return window.Compositor();
  }
  return GetCompositor();
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

comp::Compositor GetCompositor() {
  if (!react::uwp::IsXamlIsland()) {
    return xaml::Window::Current().Compositor();
  }
  comp::Compositor compositor;
  assert(tlsCompositor != nullptr);
  return tlsCompositor;
}

} // namespace Microsoft::ReactNative
