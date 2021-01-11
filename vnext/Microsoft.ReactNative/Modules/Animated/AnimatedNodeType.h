// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <string>

enum class AnimatedNodeType {
  Style,
  Value,
  Props,
  Interpolation,
  Addition,
  Subtraction,
  Division,
  Multiplication,
  Modulus,
  Diffclamp,
  Transform,
  Tracking,
};

static AnimatedNodeType AnimatedNodeTypeFromString(const std::string &string) {
  if (string == "style")
    return AnimatedNodeType::Style;
  if (string == "value")
    return AnimatedNodeType::Value;
  if (string == "props")
    return AnimatedNodeType::Props;
  if (string == "interpolation")
    return AnimatedNodeType::Interpolation;
  if (string == "addition")
    return AnimatedNodeType::Addition;
  if (string == "subtraction")
    return AnimatedNodeType::Subtraction;
  if (string == "division")
    return AnimatedNodeType::Division;
  if (string == "multiplication")
    return AnimatedNodeType::Multiplication;
  if (string == "modulus")
    return AnimatedNodeType::Modulus;
  if (string == "diffclamp")
    return AnimatedNodeType::Diffclamp;
  if (string == "transform")
    return AnimatedNodeType::Transform;

  assert(string == "tracking");
  return AnimatedNodeType::Tracking;
};
