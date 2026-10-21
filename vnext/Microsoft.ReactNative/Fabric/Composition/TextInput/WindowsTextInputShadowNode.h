// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ConcreteTextInputShadowNode.h"
#include "WindowsTextInputEventEmitter.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputState.h"

#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/core/conversions.h>

namespace facebook::react {

extern const char WindowsTextInputComponentName[];

class WindowsTextInputShadowNode final : public ConcreteTextInputShadowNode<
                                             WindowsTextInputComponentName,
                                             facebook::react::WindowsTextInputProps,
                                             facebook::react::WindowsTextInputEventEmitter,
                                             facebook::react::WindowsTextInputState> {
 public:
  using Super = ConcreteTextInputShadowNode<
      WindowsTextInputComponentName,
      facebook::react::WindowsTextInputProps,
      facebook::react::WindowsTextInputEventEmitter,
      facebook::react::WindowsTextInputState>;
  using Super::Super;

  void updateTextInputState(
      facebook::react::AttributedStringBox attributedStringBox,
      facebook::react::AttributedString &reactTreeAttributedString,
      facebook::react::ParagraphAttributes paragraphAttributes,
      int64_t mostRecentEventCount) override {
    Super::setStateData(facebook::react::WindowsTextInputState{
        attributedStringBox, reactTreeAttributedString, paragraphAttributes, mostRecentEventCount});
  }
};

} // namespace facebook::react
