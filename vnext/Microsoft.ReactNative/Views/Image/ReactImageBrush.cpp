// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImageBrush.h"

#include <UI.Composition.Effects.h>
#include <sstream>

#include "Effects.h"

namespace winrt {
using namespace winrt::Windows::Storage::Streams;
using namespace comp;
using namespace xaml;
using namespace xaml::Media;
using namespace comp::Effects;
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

void ReactImageBrush::BlurRadius(float value) {
  if (m_blurRadius != value) {
    m_blurRadius = value;
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
      comp::CompositionSurfaceBrush surfaceBrush{GetOrCreateSurfaceBrush()};
      surfaceBrush.Surface(m_loadedImageSurface);
    }

    UpdateCompositionBrush();
  }
}

void ReactImageBrush::UpdateCompositionBrush() {
  if (m_loadedImageSurface) {
    comp::CompositionSurfaceBrush surfaceBrush{GetOrCreateSurfaceBrush()};
    surfaceBrush.Stretch(ResizeModeToStretch());

    auto compositionBrush{surfaceBrush.as<comp::CompositionBrush>()};
    if (ResizeMode() == ResizeMode::Repeat || BlurRadius() > 0) {
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

comp::CompositionStretch ReactImageBrush::ResizeModeToStretch() {
  auto stretch{comp::CompositionStretch::None};

  switch (ResizeMode()) {
    case ResizeMode::Contain:
      stretch = comp::CompositionStretch::Uniform;
      break;

    case ResizeMode::Cover:
      stretch = comp::CompositionStretch::UniformToFill;
      break;

    case ResizeMode::Stretch:
      stretch = comp::CompositionStretch::Fill;
      break;

    case ResizeMode::Center:
    case ResizeMode::Repeat:
      stretch = IsImageSmallerThanView() ? comp::CompositionStretch::None : comp::CompositionStretch::Uniform;
      break;
  }

  return stretch;
}

comp::CompositionSurfaceBrush ReactImageBrush::GetOrCreateSurfaceBrush() {
  // If it doesn't exist, create it
  if (!CompositionBrush()) {
    comp::CompositionSurfaceBrush surfaceBrush{m_compositor.CreateSurfaceBrush()};
    surfaceBrush.Surface(m_loadedImageSurface);

    return surfaceBrush;
  }

  auto surfaceBrush{CompositionBrush().try_as<comp::CompositionSurfaceBrush>()};

  // If ResizeMode is set to Repeat, then we're using a CompositionEffectBrush.
  // Get the CompositionSurfaceBrush from its source.
  if (!surfaceBrush) {
    auto effectBrush{CompositionBrush().as<comp::CompositionEffectBrush>()};
    assert(effectBrush);

    surfaceBrush = effectBrush.GetSourceParameter(L"source").as<comp::CompositionSurfaceBrush>();
  }

  return surfaceBrush;
}

comp::CompositionEffectBrush ReactImageBrush::GetOrCreateEffectBrush(
    comp::CompositionSurfaceBrush const &surfaceBrush) {
  if (!m_effectBrush) {

    winrt::Windows::Graphics::Effects::IGraphicsEffect effect;

    if (ResizeMode() == ResizeMode::Repeat) {
      // BorderEffect
      auto borderEffect{winrt::make<EFFECTS_NAMESPACE::implementation::BorderEffect>()};

      borderEffect.ExtendX(winrt::Microsoft::ReactNative::CanvasEdgeBehavior::Wrap);
      borderEffect.ExtendY(winrt::Microsoft::ReactNative::CanvasEdgeBehavior::Wrap);

      comp::CompositionEffectSourceParameter borderEffectSourceParameter{L"source"};
      borderEffect.Source(borderEffectSourceParameter);
      effect = borderEffect;
    } else {
      // GaussianBlurEffect
      auto blurEffect{winrt::make<EFFECTS_NAMESPACE::implementation::GaussianBlurEffect>()};

      blurEffect.BlurAmount(m_blurRadius);

      comp::CompositionEffectSourceParameter blurEffectSourceParameter{L"source"};
      blurEffect.Source(blurEffectSourceParameter);
      effect = blurEffect;
    }

    comp::CompositionEffectFactory effectFactory{m_compositor.CreateEffectFactory(effect)};

    m_effectBrush = effectFactory.CreateBrush();

    m_effectBrush.SetSourceParameter(L"source", surfaceBrush);
  }

  return m_effectBrush;
}

} // namespace uwp
} // namespace react
