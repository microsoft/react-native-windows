// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <string>

enum class FacadeType {
  Opacity,
  Rotation,
  RotationX,
  RotationY,
  Scale,
  ScaleX,
  ScaleY,
  TranslateX,
  TranslateY,
  Perspective,
  Progress
};

inline FacadeType StringToFacadeType(const std::string &value) noexcept {
  if (value == "opacity")
    return FacadeType::Opacity;
  if (value == "rotate")
    return FacadeType::Rotation;
  if (value == "rotateX")
    return FacadeType::RotationX;
  if (value == "rotateY")
    return FacadeType::RotationY;
  if (value == "scale")
    return FacadeType::Scale;
  if (value == "scaleX")
    return FacadeType::ScaleX;
  if (value == "scaleY")
    return FacadeType::ScaleY;
  if (value == "translateX")
    return FacadeType::TranslateX;
  if (value == "translateY")
    return FacadeType::TranslateY;
  if (value == "perspective")
    return FacadeType::Perspective;

  assert(value == "progress");
  return FacadeType::Progress;
}
