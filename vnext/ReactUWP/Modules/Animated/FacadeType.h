// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <string>

enum FacadeType {
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

static FacadeType StringToFacadeType(const std::string &string) {
  if (string == "opacity")
    return FacadeType::Opacity;
  if (string == "rotate")
    return FacadeType::Rotation;
  if (string == "rotateX")
    return FacadeType::RotationX;
  if (string == "rotateY")
    return FacadeType::RotationY;
  if (string == "scale")
    return FacadeType::Scale;
  if (string == "scaleX")
    return FacadeType::ScaleX;
  if (string == "scaleY")
    return FacadeType::ScaleY;
  if (string == "translateX")
    return FacadeType::TranslateX;
  if (string == "translateY")
    return FacadeType::TranslateY;
  if (string == "perspective")
    return FacadeType::Perspective;

  assert(string == "progress");
  return FacadeType::Progress;
}
