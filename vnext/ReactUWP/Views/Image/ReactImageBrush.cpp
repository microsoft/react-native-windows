// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

namespace react {
  namespace uwp {

    ReactImageBrush::ReactImageBrush()
    {
    }

    /*static*/ winrt::com_ptr<ReactImageBrush> ReactImageBrush::Create()
    {
      return winrt::make_self<ReactImageBrush>();
    }

    void ReactImageBrush::SourceUri(winrt::Uri const& value)
    {
      if (m_sourceUri != value)
      {
        m_sourceUri = value;
        UpdateCompositionBrush();
      }
    }

    void ReactImageBrush::ResizeMode(react::uwp::ResizeMode value)
    {
      if (m_resizeMode != value)
      {
        m_resizeMode = value;
        UpdateCompositionBrush();
      }
    }

    void ReactImageBrush::AvailableSize(winrt::Size const& value)
    {
      if (m_availableSize != value)
      {
        m_availableSize = value;
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
        if (ResizeMode() == ResizeMode::Repeat)
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
        auto loadedSurface{ winrt::LoadedImageSurface::StartLoadFromUri(SourceUri()) };

        loadedSurface.LoadCompleted([this](IInspectable const&, winrt::LoadedImageSourceLoadCompletedEventArgs const& args)
        {
          if (args.Status() == winrt::LoadedImageSourceLoadStatus::Success)
          {
            m_imageLoaded = true;
            UpdateCompositionBrush();
          }
        });

        auto surfaceBrush{ winrt::Window::Current().Compositor().CreateSurfaceBrush() };
        surfaceBrush.Surface(loadedSurface);

        return surfaceBrush;
      }

      auto surfaceBrush{ CompositionBrush().try_as<winrt::CompositionSurfaceBrush>() };

      // If ResizeMode is set to Repeat, then we're using a CompositionEffectBrush.
      // Get the CompositionSurfaceBrush from its source.
      if (!surfaceBrush)
      {
        auto effectBrush{ CompositionBrush().as<winrt::CompositionEffectBrush>() };
        surfaceBrush = effectBrush.GetSourceParameter(L"source").as<winrt::CompositionSurfaceBrush>();
      }

      return surfaceBrush;
    }

    bool ReactImageBrush::ShouldSwitchCompositionBrush()
    {
      bool effectToSurfaceBrushSwitch{
        ResizeMode() != ResizeMode::Repeat &&
        !CompositionBrush().try_as<winrt::CompositionSurfaceBrush>() };

      bool surfaceToEffectBrushSwitch{ ResizeMode() == ResizeMode::Repeat };

      return effectToSurfaceBrushSwitch || surfaceToEffectBrushSwitch;
    }

    bool ReactImageBrush::IsImageLargerThanView()
    {
      if (m_imageLoaded)
      {
        auto surface{ GetOrCreateSurfaceBrush().Surface().as<winrt::LoadedImageSurface>() };
        auto dipsSize{ surface.DecodedSize() };

        return (dipsSize.Height > AvailableSize().Height) || (dipsSize.Width > AvailableSize().Width);
      }

      return false;
    }

    winrt::CompositionStretch ReactImageBrush::ResizeModeToStretch()
    {
      auto stretch{ winrt::CompositionStretch::None };

      switch (ResizeMode())
      {
      case ResizeMode::Center:
        stretch = IsImageLargerThanView() ? winrt::CompositionStretch::Uniform : winrt::CompositionStretch::None;
        break;

      case ResizeMode::Contain:
        stretch = winrt::CompositionStretch::Uniform;
        break;

      case ResizeMode::Cover:
        stretch = winrt::CompositionStretch::UniformToFill;
        break;

      case ResizeMode::Stretch:
        stretch = winrt::CompositionStretch::Fill;
        break;

      case ResizeMode::Repeat:
        stretch = IsImageLargerThanView() ? winrt::CompositionStretch::Uniform : winrt::CompositionStretch::None;
        break;
      }

      return stretch;
    }
  }
} // namespace react::uwp
