// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlView.h"
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Controls.Primitives.h>

namespace react::uwp {

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
#ifdef USE_WINUI3
  else {
    return TryGetXamlRoot(view).Compositor();
  }
#else
  throw std::exception("Could not get a compositor instance");
#endif
}

} // namespace react::uwp
