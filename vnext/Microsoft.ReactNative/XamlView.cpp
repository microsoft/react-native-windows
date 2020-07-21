// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlView.h"
#include <UI.Composition.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Documents.h>
#include "Utils/Helpers.h"

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
  else if (auto root = TryGetXamlRoot(view)) {
    return root.Compositor();
  }
#endif
  return GetCompositor();
}

static DWORD tlsCompositor = 0;
void SetCompositor(const comp::Compositor &compositor) {
  if (tlsCompositor == 0) {
    tlsCompositor = TlsAlloc();
  }
  winrt::com_ptr<IUnknown> rawCompositor(winrt::get_abi(compositor), winrt::take_ownership_from_abi);
  auto oldCompositor = TlsGetValue(tlsCompositor);
  if (oldCompositor != nullptr) {
    assert(oldCompositor == rawCompositor.get());
  } else {
    rawCompositor->AddRef();
    TlsSetValue(tlsCompositor, rawCompositor.get());
  }
}

comp::Compositor GetCompositor() {

  if (!react::uwp::IsXamlIsland()) {
    return xaml::Window::Current().Compositor();
  }
  comp::Compositor compositor;
  if (tlsCompositor != 0) {
    winrt::copy_from_abi(compositor, TlsGetValue(tlsCompositor));
  }
  assert(compositor != nullptr);
  return compositor;
}

} // namespace react::uwp
