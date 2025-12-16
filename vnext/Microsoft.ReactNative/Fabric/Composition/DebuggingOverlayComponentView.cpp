
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DebuggingOverlayComponentView.h"

#include <JSValueComposition.h>
#include <JSValueReader.h>
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

DebuggingOverlayComponentView::DebuggingOverlayComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          {}, // default ViewProps
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder | ComponentViewFeatures::FocusVisual)) {}

void DebuggingOverlayComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void DebuggingOverlayComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

winrt::Microsoft::ReactNative::ComponentView DebuggingOverlayComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<DebuggingOverlayComponentView>(compContext, tag, reactContext);
}

REACT_STRUCT(ElementRectangle)
struct ElementRectangle {
  REACT_FIELD(x)
  float x;
  REACT_FIELD(y)
  float y;
  REACT_FIELD(width)
  float width;
  REACT_FIELD(height)
  float height;
};

REACT_STRUCT(TraceUpdate)
struct TraceUpdate {
  REACT_FIELD(id)
  double id;
  REACT_FIELD(rectangle)
  ElementRectangle rectangle;
  REACT_FIELD(color)
  winrt::Microsoft::ReactNative::Color color{nullptr};
};

void DebuggingOverlayComponentView::HandleCommand(
    const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  base_type::HandleCommand(args);
  if (args.Handled())
    return;

  auto commandName = args.CommandName();
  if (commandName == L"highlightTraceUpdates") {
    std::vector<TraceUpdate> updates;
    winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), updates);
    // TODO should create visuals that get removed after 2 seconds
    return;
  }
  if (commandName == L"highlightElements") {
    std::vector<ElementRectangle> elements;
    winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), elements);

    if (auto root = rootComponentView()) {
      auto rootVisual = root->OuterVisual();
      auto brush = m_compContext.CreateColorBrush({204, 200, 230, 255});
      auto scaleFactor = root->theme()->PointScaleFactor();
      for (auto &element : elements) {
        auto overlayVisual = m_compContext.CreateSpriteVisual();
        overlayVisual.Size({element.width * scaleFactor, element.height * scaleFactor});
        overlayVisual.Offset({element.x * scaleFactor, element.y * scaleFactor, 0.0f});
        overlayVisual.Brush(brush);

        rootVisual.InsertAt(overlayVisual, root->overlayIndex() + m_activeOverlays);
        ++m_activeOverlays;
      }
    }
    return;
  }
  if (commandName == L"clearElementsHighlights") {
    if (auto root = rootComponentView()) {
      auto rootVisual = root->OuterVisual();

      while (m_activeOverlays != 0) {
        --m_activeOverlays;
        auto visual = rootVisual.GetAt(root->overlayIndex() + m_activeOverlays);
        rootVisual.Remove(visual);
      }
    }
    return;
  }
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
