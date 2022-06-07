// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextVisitor.h"

namespace Microsoft::ReactNative {

class TextHitTestVisitor : public TextVisitor {
  using Super = TextVisitor;

 public:
  TextHitTestVisitor(winrt::Microsoft::ReactNative::ReactPointerEventArgs const &args, winrt::Point const &point)
      : Super(), m_args{args}, m_point{point} {}

 protected:
  void VisitCore(ShadowNodeBase *node) override;

  void VisitRawText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  winrt::Microsoft::ReactNative::ReactPointerEventArgs const &m_args;
  winrt::Point const &m_point;
  bool m_hasPressableAncestor{false};
  int m_pressableCount{0};
  bool m_targetFound{false};
};

} // namespace Microsoft::ReactNative
