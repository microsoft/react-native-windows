// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "DebuggerUIIsland.h"

#include <AutoDraw.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include "CompositionContextHelper.h"
#include "TextDrawing.h"

namespace winrt::Microsoft::ReactNative::implementation {

constexpr float debuggerUIFontSize = 10.0f;
constexpr float debuggerTextMargin = 4.0f;

DebuggerUIIsland::DebuggerUIIsland(
    const winrt::Microsoft::UI::Composition::Compositor &compositor,
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept
    : m_compositor(compositor), m_compContext(compContext), m_theme(theme) {
  m_backgroundVisual = m_compositor.CreateSpriteVisual();
  m_backgroundVisual.RelativeSizeAdjustment({1.0f, 1.0f});

  auto backgroundBrush = m_compositor.CreateColorBrush({100, 0, 0, 0});
  m_backgroundVisual.Brush(backgroundBrush);

  m_TextVisual = m_compositor.CreateSpriteVisual();
  m_TextVisual.IsPixelSnappingEnabled(true);

  m_backgroundVisual.Children().InsertAtTop(m_TextVisual);
}

DebuggerUIIsland::~DebuggerUIIsland() noexcept {
  m_island.StateChanged(m_islandStateChangedToken);
}

void DebuggerUIIsland::Redraw() noexcept {
  if (!m_island)
    return;

  if (m_island.ActualSize().x == 0 || m_island.ActualSize().y == 0)
    return;

  auto scaleFactor = m_island.Environment().DisplayScale();

  auto attributedString = facebook::react::AttributedString{};
  auto fragment = facebook::react::AttributedString::Fragment{};
  fragment.string = m_message;
  fragment.textAttributes.fontSize = debuggerUIFontSize;
  attributedString.appendFragment(std::move(fragment));

  // Resume Icon
  auto iconFragment = facebook::react::AttributedString::Fragment{};
  iconFragment.string = " \uF08F";
  iconFragment.textAttributes.fontFamily = "Segoe Fluent Icons";
  iconFragment.textAttributes.fontSize = debuggerUIFontSize;
  attributedString.appendFragment(std::move(iconFragment));

  auto attributedStringBox = facebook::react::AttributedStringBox{attributedString};

  facebook::react::LayoutConstraints constraints;
  constraints.maximumSize.width = std::max(0.0f, m_island.ActualSize().x - debuggerTextMargin * 2 * scaleFactor);
  constraints.maximumSize.height = std::max(0.0f, m_island.ActualSize().y - debuggerTextMargin * 2 * scaleFactor);

  auto textAttributes = facebook::react::TextAttributes{};
  textAttributes.foregroundColor = facebook::react::blackColor();

  winrt::com_ptr<::IDWriteTextLayout> textLayout;
  facebook::react::WindowsTextLayoutManager::GetTextLayout(attributedStringBox, {}, constraints, textLayout);

  DWRITE_TEXT_METRICS tm;
  textLayout->GetMetrics(&tm);

  winrt::Windows::Foundation::Size surfaceSize = {
      std::ceilf(std::min(constraints.maximumSize.width, tm.width + debuggerTextMargin * 2 * scaleFactor)),
      std::ceilf(std::min(constraints.maximumSize.height, tm.height + debuggerTextMargin * 2 * scaleFactor))};
  auto drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
      surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

  POINT offset;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingSurface, scaleFactor, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(D2D1::ColorF{1.0f, 1.0f, 0.76f, 1.0f});

      auto theme = winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(m_theme);

      Composition::RenderText(
          *d2dDeviceContext,
          *textLayout,
          attributedStringBox.getValue(),
          textAttributes,
          {static_cast<float>(offset.x + std::floorf(debuggerTextMargin * scaleFactor)),
           static_cast<float>(offset.y + std::floorf(debuggerTextMargin * scaleFactor))},
          scaleFactor,
          *theme);
    }

    drawingSurface.HorizontalAlignmentRatio(0.0f);
    drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);

    m_TextVisual.Brush(winrt::Microsoft::ReactNative::Composition::Experimental::implementation::
                           MicrosoftCompositionContextHelper::InnerBrush(drawingSurface));
    m_TextVisual.Size({surfaceSize.Width, surfaceSize.Height});

    m_debuggerHitRect = {
        m_island.ActualSize().x / 2 - tm.width / 2 + debuggerTextMargin * scaleFactor,
        debuggerTextMargin * scaleFactor,
        surfaceSize.Width,
        surfaceSize.Height};

    m_TextVisual.Offset({m_debuggerHitRect.X, m_debuggerHitRect.Y, 0.0f});
  }
}

void DebuggerUIIsland::Message(std::string &&value) noexcept {
  m_message = value;
  Redraw();
}

winrt::Microsoft::UI::Content::ContentIsland DebuggerUIIsland::Island() noexcept {
  if (!m_island) {
    m_island = winrt::Microsoft::UI::Content::ContentIsland::Create(m_backgroundVisual);

    m_islandStateChangedToken =
        m_island.StateChanged([weakThis = weak_from_this()](
                                  winrt::Microsoft::UI::Content::ContentIsland const &island,
                                  winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const &args) {
          if (auto pThis = weakThis.lock()) {
            if (args.DidRasterizationScaleChange() || args.DidActualSizeChange()) {
              pThis->Redraw();
            }
          }
        });

    auto pointerSource = winrt::Microsoft::UI::Input::InputPointerSource::GetForIsland(m_island);

    m_islandPointerUpToken =
        pointerSource.PointerReleased([weakThis = weak_from_this()](
                                          winrt::Microsoft::UI::Input::InputPointerSource const &,
                                          winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
          if (auto pThis = weakThis.lock()) {
            auto position = args.CurrentPoint().Position();
            if (position.X >= pThis->m_debuggerHitRect.X && position.Y >= pThis->m_debuggerHitRect.Y &&
                position.X <= pThis->m_debuggerHitRect.X + pThis->m_debuggerHitRect.Width &&
                position.Y <= pThis->m_debuggerHitRect.Y + pThis->m_debuggerHitRect.Height) {
              pThis->m_resumedEvent(nullptr, nullptr);
            }
          }
        });
  }
  return m_island;
}

winrt::event_token DebuggerUIIsland::Resumed(
    winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const &handler) noexcept {
  return m_resumedEvent.add(handler);
}
void DebuggerUIIsland::Resumed(winrt::event_token const &token) noexcept {
  m_resumedEvent.remove(token);
}

} // namespace winrt::Microsoft::ReactNative::implementation
