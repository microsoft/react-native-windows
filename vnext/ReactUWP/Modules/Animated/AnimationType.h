// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <string>

enum AnimationType {
  Decay,
  Frames,
  Spring,
};

static AnimationType AnimationTypeFromString(const std::string &string) {
  if (string == "decay")
    return AnimationType::Decay;
  if (string == "frames")
    return AnimationType::Frames;

  assert(string == "spring");
  return AnimationType::Spring;
}
