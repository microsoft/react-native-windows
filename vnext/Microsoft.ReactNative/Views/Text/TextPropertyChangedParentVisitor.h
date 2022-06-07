// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/TextViewManager.h>
#include "TextParentVisitor.h"

namespace Microsoft::ReactNative {

enum class PropertyChangeType : std::uint_fast8_t {
  None = 0,
  Text = 1 << 0,
  AddHighlight = 1 << 1,
  RemoveHighlight = 1 << 2,
  AddPressable = 1 << 3,
};

DEFINE_ENUM_FLAG_OPERATORS(PropertyChangeType);

class TextPropertyChangedParentVisitor : public TextParentVisitor {
  using Super = TextParentVisitor;

 public:
  TextPropertyChangedParentVisitor(PropertyChangeType type) : m_propertyChangeType{type} {}

 protected:
  void VisitCore(ShadowNodeBase *node) override;

  void VisitText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  PropertyChangeType m_propertyChangeType;
  bool m_isNested{false};

  bool HasPropertyChangeType(PropertyChangeType type) {
    return (m_propertyChangeType & type) == type;
  }
};

} // namespace Microsoft::ReactNative
