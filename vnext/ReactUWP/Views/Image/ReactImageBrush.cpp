// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImageBrush.h"

#include <sstream>

#include <winrt/Windows.UI.Composition.h>

#include "BorderEffect.h"

namespace winrt {
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {
/*static*/ winrt::com_ptr<ReactImageBrush> ReactImageBrush::Create() {
  return winrt::make_self<ReactImageBrush>();
}

void ReactImageBrush::OnConnected() {
  UpdateCompositionBrush();
}

void ReactImageBrush::OnDisconnected() {
  // Dispose of composition resources when no longer in use.
  if (CompositionBrush()) {
    CompositionBrush(nullptr);
  }
}

void ReactImageBrush::ResizeMode(react::uwp::ResizeMode value) {
  if (m_resizeMode != value) {
    m_resizeMode = value;
    UpdateCompositionBrush();
  }
}

void ReactImageBrush::AvailableSize(winrt::Size const &value) {
  if (m_availableSize != value) {
    m_availableSize = value;
    UpdateCompositionBrush();
  }
}

void ReactImageBrush::Source(winrt::LoadedImageSurface const &value) {
  if (m_loadedImageSurface != value) {
    bool updateSurface{m_loadedImageSurface};

    m_loadedImageSurface = value;

    if (updateSurface) {
      winrt::CompositionSurfaceBrush surfaceBrush{GetOrCreateSurfaceBrush()};
      surfaceBrush.Surface(m_loadedImageSurface);
    }

    UpdateCompositionBrush();
  }
}

void ReactImageBrush::UpdateCompositionBrush() {
  if (m_loadedImageSurface) {
    winrt::CompositionSurfaceBrush surfaceBrush{GetOrCreateSurfaceBrush()};
    surfaceBrush.Stretch(ResizeModeToStretch());

    auto compositionBrush{surfaceBrush.as<winrt::CompositionBrush>()};
    if (ResizeMode() == ResizeMode::Repeat) {
      // If ResizeMode is set to Repeat, then we need to use a CompositionEffectBrush.
      // The CompositionSurfaceBrush holding the image is used as its source.
      compositionBrush = GetOrCreateEffectBrush(surfaceBrush);
    }

    // The CompositionBrush is only set after the image is first loaded and anytime
    // we switch between Surface and Effect brushes (to/from ResizeMode::Repeat)
    if (CompositionBrush() != compositionBrush) {
      if (ResizeMode() == ResizeMode::Repeat) {
        surfaceBrush.HorizontalAlignmentRatio(0.0f);
        surfaceBrush.VerticalAlignmentRatio(0.0f);
      } else {
        surfaceBrush.HorizontalAlignmentRatio(0.5f);
        surfaceBrush.VerticalAlignmentRatio(0.5f);
      }

      CompositionBrush(compositionBrush);
    }
  }
}

bool ReactImageBrush::IsImageSmallerThanView() {
  if (m_loadedImageSurface) {
    auto surface{GetOrCreateSurfaceBrush().Surface().as<winrt::LoadedImageSurface>()};
    winrt::Size dipsSize{surface.DecodedSize()};

    return (dipsSize.Height < AvailableSize().Height) && (dipsSize.Width < AvailableSize().Width);
  }

  return false;
}

winrt::CompositionStretch ReactImageBrush::ResizeModeToStretch() {
  auto stretch{winrt::CompositionStretch::None};

  switch (ResizeMode()) {
    case ResizeMode::Contain:
      stretch = winrt::CompositionStretch::Uniform;
      break;

    case ResizeMode::Cover:
      stretch = winrt::CompositionStretch::UniformToFill;
      break;

    case ResizeMode::Stretch:
      stretch = winrt::CompositionStretch::Fill;
      break;

    case ResizeMode::Center:
    case ResizeMode::Repeat:
      stretch = IsImageSmallerThanView() ? winrt::CompositionStretch::None : winrt::CompositionStretch::Uniform;
      break;
  }

  return stretch;
}

winrt::CompositionSurfaceBrush ReactImageBrush::GetOrCreateSurfaceBrush() {
  // If it doesn't exist, create it
  if (!CompositionBrush()) {
    winrt::CompositionSurfaceBrush surfaceBrush{winrt::Window::Current().Compositor().CreateSurfaceBrush()};
    surfaceBrush.Surface(m_loadedImageSurface);

    return surfaceBrush;
  }

  auto surfaceBrush{CompositionBrush().try_as<winrt::CompositionSurfaceBrush>()};

  // If ResizeMode is set to Repeat, then we're using a CompositionEffectBrush.
  // Get the CompositionSurfaceBrush from its source.
  if (!surfaceBrush) {
    auto effectBrush{CompositionBrush().as<winrt::CompositionEffectBrush>()};
    assert(effectBrush);

    surfaceBrush = effectBrush.GetSourceParameter(L"source").as<winrt::CompositionSurfaceBrush>();
  }

  return surfaceBrush;
}

winrt::CompositionEffectBrush ReactImageBrush::GetOrCreateEffectBrush(
    winrt::CompositionSurfaceBrush const &surfaceBrush) {
  if (!m_effectBrush) {
    auto borderEffect{winrt::make<winrt::implementation::BorderEffect>()};
    borderEffect.ExtendX(winrt::CanvasEdgeBehavior::Wrap);
    borderEffect.ExtendY(winrt::CanvasEdgeBehavior::Wrap);

    winrt::CompositionEffectSourceParameter borderEffectSourceParameter{L"source"};
    borderEffect.Source(borderEffectSourceParameter);

    winrt::CompositionEffectFactory effectFactory{
        winrt::Window::Current().Compositor().CreateEffectFactory(borderEffect)};
    m_effectBrush = effectFactory.CreateBrush();

    m_effectBrush.SetSourceParameter(L"source", surfaceBrush);
  }

  return m_effectBrush;
}

} // namespace uwp
} // namespace react
