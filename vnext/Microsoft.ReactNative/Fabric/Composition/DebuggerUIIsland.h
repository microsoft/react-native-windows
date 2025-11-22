// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct DebuggerUIIsland : std::enable_shared_from_this<DebuggerUIIsland> {
  DebuggerUIIsland(
      const winrt::Microsoft::UI::Composition::Compositor &compositor,
      winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      winrt::Microsoft::ReactNative::Composition::Theme theme) noexcept;
  ~DebuggerUIIsland() noexcept;
  winrt::Microsoft::UI::Content::ContentIsland Island() noexcept;

  void Message(std::string &&value) noexcept;

  winrt::event_token Resumed(
      winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const &handler) noexcept;
  void Resumed(winrt::event_token const &token) noexcept;

 private:
  void Redraw() noexcept;

  winrt::event_token m_islandStateChangedToken;
  winrt::event_token m_islandPointerUpToken;

  winrt::Microsoft::UI::Composition::SpriteVisual m_backgroundVisual{nullptr};
  winrt::Microsoft::UI::Composition::SpriteVisual m_TextVisual{nullptr};
  winrt::Windows::Foundation::Rect m_debuggerHitRect{0, 0, 0, 0};
  winrt::Microsoft::ReactNative::Composition::Theme m_theme{nullptr};
  std::string m_message;

  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> m_resumedEvent;
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compContext{nullptr};
  winrt::Microsoft::UI::Content::ContentIsland m_island{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation
