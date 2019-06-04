#pragma once
#include <Views/cppwinrt/react.uwp.image.ReactImageBrush.g.h>

namespace winrt::react::uwp::image::implementation
{
  struct ReactImageBrush : ReactImageBrushT<ReactImageBrush>
  {
    ReactImageBrush() = default;

    react::uwp::image::ResizeMode ResizeMode() { return m_resizeMode; }
    void ResizeMode(react::uwp::image::ResizeMode value);

    Windows::Foundation::Uri SourceUri() { return m_sourceUri; }
    void SourceUri(Windows::Foundation::Uri const& value) { m_sourceUri = value; }

    // XamlCompositionBaseBrush
    void OnConnected();
    void OnDisconnected();

  private:
    bool m_usingEffectBrush{ false };
    Windows::Foundation::Uri m_sourceUri{ nullptr };
    react::uwp::image::ResizeMode m_resizeMode{ react::uwp::image::ResizeMode::Contain };
    Windows::UI::Composition::CompositionEffectBrush m_effectBrush{ nullptr };

    bool IsImageLargerThanView();
    bool ShouldSwitchCompositionBrush();
    void UpdateCompositionBrush();
    Windows::UI::Composition::CompositionStretch ResizeModeToStretch();
    Windows::UI::Composition::CompositionSurfaceBrush GetOrCreateSurfaceBrush();
    Windows::UI::Composition::CompositionEffectBrush GetOrCreateEffectBrush(Windows::UI::Composition::CompositionSurfaceBrush const& surfaceBrush);

  };
}

namespace winrt::react::uwp::image::factory_implementation
{
  struct ReactImageBrush : ReactImageBrushT<ReactImageBrush, implementation::ReactImageBrush>
  {
  };
}
