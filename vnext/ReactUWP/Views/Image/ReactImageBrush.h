// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react {
  namespace uwp {

    enum class ResizeMode
    {
      Cover = 0,
      Contain = 1,
      Stretch = 2,
      Repeat = 3,
      Center = 4,
    };

    struct ReactImageBrush : winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush>
    {
      using Super = winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush>;
    private:
      // Constructors
      ReactImageBrush();

    public:
      static winrt::com_ptr<ReactImageBrush> Create();
      template <typename D, typename... Args> friend auto winrt::make_self(Args&& ... args);

      // Public Properties
      react::uwp::ResizeMode ResizeMode() { return m_resizeMode; }
      void ResizeMode(react::uwp::ResizeMode value);

      winrt::Windows::Foundation::Uri SourceUri() { return m_sourceUri; }
      void SourceUri(winrt::Windows::Foundation::Uri const& value);

      winrt::Windows::Foundation::Size AvailableSize() { return m_availableSize; }
      void AvailableSize(winrt::Windows::Foundation::Size const& value);

      // XamlCompositionBaseBrush Methods
      void OnConnected();
      void OnDisconnected();

    private:
      bool m_imageLoaded{ false };
      winrt::Windows::Foundation::Uri m_sourceUri{ nullptr };
      winrt::Windows::Foundation::Size m_availableSize{ };
      react::uwp::ResizeMode m_resizeMode{ ResizeMode::Contain };
      winrt::Windows::UI::Composition::CompositionEffectBrush m_effectBrush{ nullptr };

      bool IsImageLargerThanView();
      bool ShouldSwitchCompositionBrush();
      void UpdateCompositionBrush();
      winrt::Windows::UI::Composition::CompositionStretch ResizeModeToStretch();
      winrt::Windows::UI::Composition::CompositionSurfaceBrush GetOrCreateSurfaceBrush();
      winrt::Windows::UI::Composition::CompositionEffectBrush GetOrCreateEffectBrush(winrt::Windows::UI::Composition::CompositionSurfaceBrush const& surfaceBrush);
    };
  }
} // namespace react::uwp
