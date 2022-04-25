// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputShadowNode.h"

#include <MsoReactContext.h>
#include <yoga/CompactValue.h>
#include <yoga/YGEnums.h>
#include <yoga/YGValue.h>

#include <react/renderer/core/ConcreteComponentDescriptor.h>

namespace facebook {
namespace react {

void InitTextInputThemeInfo(const Mso::React::IReactContext &reactContext);

/*
 * Descriptor for <WindowsTextInput> component.
 */
class WindowsTextInputComponentDescriptor final : public ConcreteComponentDescriptor<WindowsTextInputShadowNode> {
 public:
  WindowsTextInputComponentDescriptor(ComponentDescriptorParameters const &parameters);

  virtual State::Shared createInitialState(ShadowNodeFragment const &fragment, ShadowNodeFamily::Shared const &family)
      const override;

 protected:
  void adopt(ShadowNode::Unshared const &shadowNode) const override;

 private:
  SharedTextLayoutManager textLayoutManager_;
};

} // namespace react
} // namespace facebook
