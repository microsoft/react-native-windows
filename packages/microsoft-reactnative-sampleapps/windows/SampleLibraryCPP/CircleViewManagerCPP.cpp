// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CircleViewManagerCPP.h"

using namespace winrt;
using namespace Microsoft::ReactNative::Bridge;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;

namespace winrt::SampleLibraryCPP::implementation {

CircleViewManagerCPP::CircleViewManagerCPP() {}

// IViewManager
hstring CircleViewManagerCPP::Name() noexcept {
  return L"CircleCPP";
}

FrameworkElement CircleViewManagerCPP::CreateView() noexcept {
  auto const &view = winrt::Windows::UI::Xaml::Controls::Border();

  auto const &binding = winrt::Windows::UI::Xaml::Data::Binding();
  binding.Source(view);
  binding.Path(winrt::Windows::UI::Xaml::PropertyPath(L"Height"));
  binding.Converter(HeightToCornerRadiusConverter::Instance());

  view.SetBinding(winrt::Windows::UI::Xaml::Controls::Border::CornerRadiusProperty(), binding);

  return view;
}

// IViewManagerWithChildren

void CircleViewManagerCPP::AddView(FrameworkElement const &parent, UIElement const &child, int64_t index) noexcept {
  if (auto const &border = parent.try_as<Border>()) {
    border.Child(child);
  }
}

void CircleViewManagerCPP::RemoveAllChildren(FrameworkElement const &parent) noexcept {
  if (auto const &border = parent.try_as<Border>()) {
    border.Child(nullptr);
  }
}

void CircleViewManagerCPP::RemoveChildAt(FrameworkElement const &parent, int64_t index) noexcept {
  if (auto const &border = parent.try_as<Border>()) {
    border.Child(nullptr);
  }
}

void CircleViewManagerCPP::ReplaceChild(
    FrameworkElement const &parent,
    UIElement const &oldChild,
    UIElement const &newChild) noexcept {
  if (auto const &border = parent.try_as<Border>()) {
    border.Child(newChild);
  }
}

} // namespace winrt::SampleLibraryCPP::implementation
