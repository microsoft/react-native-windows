// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <string>
#include <cassert>

enum FacadeType
{
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
};

static FacadeType StringToFacadeType(const std::string& string)
{
  if (string == "opacity") return FacadeType::Opacity;
  if (string == "rotate") return FacadeType::Rotation;
  if (string == "rotateX") return FacadeType::RotationX;
  if (string == "rotateY") return FacadeType::RotationY;
  if (string == "scale") return FacadeType::Scale;
  if (string == "scaleX") return FacadeType::ScaleX;
  if (string == "scaleY") return FacadeType::ScaleY;
  if (string == "translateX") return FacadeType::TranslateX;
  if (string == "translateY") return FacadeType::TranslateY;

  assert(string == "perspective");
  return FacadeType::Perspective;
}
