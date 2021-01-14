// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Composition.Effects.h>
#include "Effects.h"
#include "ReactImageBrush.h"
#include "XamlView.h"

#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <sstream>

namespace winrt {
using namespace winrt::Windows::Storage::Streams;
using namespace comp;
using namespace xaml;
using namespace xaml::Media;
using namespace comp::Effects;
} // namespace winrt

namespace react::uwp {
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
    const bool forceEffectBrush{value == ResizeMode::Repeat || m_resizeMode == ResizeMode::Repeat};
    m_resizeMode = value;
    UpdateCompositionBrush(forceEffectBrush);
  }
}

void ReactImageBrush::BlurRadius(float value) {
  if (m_blurRadius != value) {
    m_blurRadius = value;
    UpdateCompositionBrush();
  }
}

void ReactImageBrush::TintColor(winrt::Color value) {
  bool sameColor{
      value.A == m_tintColor.A && value.R == m_tintColor.R && value.G == m_tintColor.G && value.B == m_tintColor.B};

  if (!sameColor) {
    const bool forceEffectBrush{value.A == 0 || m_tintColor.A == 0};
    m_tintColor = value;
    UpdateCompositionBrush(forceEffectBrush);
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

void ReactImageBrush::UpdateCompositionBrush(bool forceEffectBrush) {
  if (m_loadedImageSurface) {
    comp::CompositionSurfaceBrush surfaceBrush{GetOrCreateSurfaceBrush()};
    surfaceBrush.Stretch(ResizeModeToStretch());

    auto compositionBrush{surfaceBrush.as<comp::CompositionBrush>()};
    if (ResizeMode() == ResizeMode::Repeat || BlurRadius() > 0 || m_tintColor.A != 0) {
      // The effects used for ResizeMode::Repeat and tintColor are conditional, so if we
      // are switching to/from those modes, then we need to create a new CompositionBrush.
      // The CompositionSurfaceBrush holding the image is used as its source.
      compositionBrush = GetOrCreateEffectBrush(surfaceBrush, forceEffectBrush);

      // https://microsoft.github.io/Win2D/html/P_Microsoft_Graphics_Canvas_Effects_GaussianBlurEffect_BlurAmount.htm
      // "You can compute the blur radius of the kernel by multiplying the standard deviation by 3.
      // The units of both the standard deviation and blur radius are DIPs.
      // A value of zero DIPs disables this effect entirely."
      compositionBrush.Properties().InsertScalar(BlurBlurAmount, m_blurRadius / 3);

      if (m_tintColor.A != 0) {
        compositionBrush.Properties().InsertColor(TintColorColor, m_tintColor);
      }
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
    comp::CompositionSurfaceBrush surfaceBrush{Microsoft::ReactNative::GetCompositor().CreateSurfaceBrush()};
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
    comp::CompositionSurfaceBrush const &surfaceBrush,
    bool forceEffectBrush) {
  if (!m_effectBrush || forceEffectBrush) {
    // GaussianBlurEffect
    auto blurEffect{winrt::make<winrt::Microsoft::ReactNative::implementation::GaussianBlurEffect>()};
    blurEffect.Name(L"Blur");

    // https://microsoft.github.io/Win2D/html/P_Microsoft_Graphics_Canvas_Effects_GaussianBlurEffect_BlurAmount.htm
    // "You can compute the blur radius of the kernel by multiplying the standard deviation by 3.
    // The units of both the standard deviation and blur radius are DIPs.
    // A value of zero DIPs disables this effect entirely."
    blurEffect.BlurAmount(m_blurRadius / 3);

    std::vector<winrt::hstring> animatedProperties;
    animatedProperties.push_back({BlurBlurAmount});

    if (ResizeMode() == ResizeMode::Repeat) {
      // BorderEffect
      auto borderEffect{winrt::make<winrt::Microsoft::ReactNative::implementation::BorderEffect>()};

      borderEffect.ExtendX(winrt::Microsoft::ReactNative::CanvasEdgeBehavior::Wrap);
      borderEffect.ExtendY(winrt::Microsoft::ReactNative::CanvasEdgeBehavior::Wrap);

      comp::CompositionEffectSourceParameter borderEffectSourceParameter{L"source"};
      borderEffect.Source(borderEffectSourceParameter);
      blurEffect.Source(borderEffect);
    } else {
      comp::CompositionEffectSourceParameter blurEffectSourceParameter{L"source"};
      blurEffect.Source(blurEffectSourceParameter);
    }

    winrt::IGraphicsEffect effect{blurEffect};

    // tintColor
    if (m_tintColor.A != 0) {
      auto tintColorEffect{winrt::make<winrt::Microsoft::ReactNative::implementation::ColorSourceEffect>()};
      tintColorEffect.Name(L"TintColor");
      tintColorEffect.Color(m_tintColor);

      auto compositeEffect{winrt::make<winrt::Microsoft::ReactNative::implementation::CompositeStepEffect>()};
      compositeEffect.Mode(winrt::Microsoft::ReactNative::CanvasComposite::SourceIn);
      compositeEffect.Destination(blurEffect);
      compositeEffect.Source(tintColorEffect);

      animatedProperties.push_back({TintColorColor});

      effect = compositeEffect;
    }

    comp::CompositionEffectFactory effectFactory{
        Microsoft::ReactNative::GetCompositor().CreateEffectFactory(effect, animatedProperties)};

    m_effectBrush = effectFactory.CreateBrush();
    m_effectBrush.SetSourceParameter(L"source", surfaceBrush);
  }

  return m_effectBrush;
}

} // namespace react::uwp
