
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

namespace Microsoft::ReactNative {

/**
 * Lightweight wrapper class around a ComponentView with a react tag which registers a
 * constant react tag at initialization time for a stable hash and provides the
 * underlying view to a caller if that underlying view's react tag has not
 * changed from the one provided at initalization time (i.e. recycled).
 */
struct ReactTaggedView {
  ReactTaggedView(std::shared_ptr<IComponentView> componentView) {
    m_view = std::move(componentView);
    m_tag = m_view->tag();
  }

  std::shared_ptr<IComponentView> &view() noexcept {
    if (m_view && m_view->tag() != m_tag) {
      m_view = nullptr;
    }
    return m_view;
  }

 private:
  facebook::react::Tag m_tag;
  std::shared_ptr<IComponentView> m_view;
};

} // namespace Microsoft::ReactNative
