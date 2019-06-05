#include "pch.h"

#include "BorderEffect.h"
#include "ReactImageBrush.h"
#include <winrt/Windows.UI.Composition.h>

#include <sstream>

namespace winrt {
  using namespace winrt::Windows::UI::Composition;
  using namespace winrt::Windows::UI::Xaml;
  using namespace winrt::Windows::UI::Xaml::Media;
}

using namespace winrt::react::uwp;

namespace winrt::react::uwp::image::implementation
{
  void ReactImageBrush::SourceUri(Windows::Foundation::Uri const& value)
  {
    if (m_sourceUri != value)
    {
      m_sourceUri = value;
      UpdateCompositionBrush();
    }
  }

  void ReactImageBrush::ResizeMode(image::ResizeMode value)
  {
    if (m_resizeMode != value)
    {
      m_resizeMode = value;
      UpdateCompositionBrush();
    }
  }

  void ReactImageBrush::OnConnected()
  {
    UpdateCompositionBrush();
  }

  void ReactImageBrush::OnDisconnected()
  {
    // Dispose of composition resources when no longer in use.
    if (CompositionBrush())
    {
      CompositionBrush(nullptr);
    }
  }

  void ReactImageBrush::UpdateCompositionBrush()
  {
    if (SourceUri())
    {
      winrt::CompositionSurfaceBrush surfaceBrush{ GetOrCreateSurfaceBrush() };
      surfaceBrush.Stretch(ResizeModeToStretch());

      auto compositionBrush{ surfaceBrush.as<winrt::CompositionBrush>() };
      if (ResizeMode() == image::ResizeMode::Repeat)
      {
        compositionBrush = GetOrCreateEffectBrush(surfaceBrush);
      }

      // Only switch CompositionBrush if we are switching to/from ResizeMode::Repeat
      if (ShouldSwitchCompositionBrush())
      {
        CompositionBrush(compositionBrush);
      }
    }
  }

  winrt::CompositionEffectBrush ReactImageBrush::GetOrCreateEffectBrush(winrt::CompositionSurfaceBrush const& surfaceBrush)
  {
    if (!m_effectBrush)
    {
      auto borderEffect{ winrt::make<winrt::implementation::BorderEffect>() };
      borderEffect.ExtendX(winrt::CanvasEdgeBehavior::Wrap);
      borderEffect.ExtendY(winrt::CanvasEdgeBehavior::Wrap);

      winrt::CompositionEffectSourceParameter borderEffectSourceParameter{ L"source" };
      borderEffect.Source(borderEffectSourceParameter);

      auto effectFactory{ winrt::Window::Current().Compositor().CreateEffectFactory(borderEffect) };
      m_effectBrush = effectFactory.CreateBrush();

      surfaceBrush.HorizontalAlignmentRatio(0.0f);
      surfaceBrush.VerticalAlignmentRatio(0.0f);

      m_effectBrush.SetSourceParameter(L"source", surfaceBrush);
    }

    return m_effectBrush;
  }

  winrt::CompositionSurfaceBrush ReactImageBrush::GetOrCreateSurfaceBrush()
  {
    // If it doesn't exist, create it
    if (!CompositionBrush())
    {
      auto surfaceBrush{ winrt::Window::Current().Compositor().CreateSurfaceBrush() };
      auto uri{ SourceUri() };
      auto loadedSurface{ winrt::LoadedImageSurface::StartLoadFromUri(SourceUri()) };

      surfaceBrush.Surface(loadedSurface);

      return surfaceBrush;
    }

    auto brush{ CompositionBrush().try_as<winrt::CompositionSurfaceBrush>() };

    // If ResizeMode is set to Repeat, then we're using a CompositionEffectBrush.
    // Get the CompositionSurfaceBrush from its source.
    if (!brush)
    {
      auto effectBrush{ CompositionBrush().as<winrt::CompositionEffectBrush>() };
      brush = effectBrush.GetSourceParameter(L"source").as<winrt::CompositionSurfaceBrush>();
    }

    return brush;
  }

  bool ReactImageBrush::ShouldSwitchCompositionBrush()
  {
    bool effectToSurfaceBrushSwitch{
      ResizeMode() != image::ResizeMode::Repeat &&
      !CompositionBrush().try_as<winrt::CompositionSurfaceBrush>() };

    bool surfaceToEffectBrushSwitch{ ResizeMode() == image::ResizeMode::Repeat };

    return effectToSurfaceBrushSwitch || surfaceToEffectBrushSwitch;
  }

  bool ReactImageBrush::IsImageLargerThanView()
  {
    bool value = false;
    return value;
  }

  winrt::CompositionStretch ReactImageBrush::ResizeModeToStretch()
  {
    auto stretch{ winrt::CompositionStretch::None };

    switch (ResizeMode())
    {
    case image::ResizeMode::Center:
      stretch = IsImageLargerThanView() ? winrt::CompositionStretch::Uniform : winrt::CompositionStretch::None;
      break;

    case image::ResizeMode::Contain:
      stretch = winrt::CompositionStretch::Uniform;
      break;

    case image::ResizeMode::Cover:
      stretch = winrt::CompositionStretch::UniformToFill;
      break;

    case image::ResizeMode::Stretch:
      stretch = winrt::CompositionStretch::Fill;
      break;

    case image::ResizeMode::Repeat:
      stretch = IsImageLargerThanView() ? winrt::CompositionStretch::Uniform : winrt::CompositionStretch::None;
      break;
    }

    return stretch;
  }
}
