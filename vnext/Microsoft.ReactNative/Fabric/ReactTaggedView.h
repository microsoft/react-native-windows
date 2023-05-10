
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

namespace Microsoft::ReactNative {

/**
 * Lightweight wrapper class around a ComponentView with a react tag which registers a
 * constant react tag at initialization time for a stable hash and provides the
 * underlying view to a caller if that underlying view's react tag has not
 * changed from the one provided at initialization time (i.e. recycled).
 */
struct ReactTaggedView {
  ReactTaggedView(const std::shared_ptr<IComponentView> &componentView)
      : m_view(componentView), m_tag(componentView->tag()) {}

  std::shared_ptr<IComponentView> view() noexcept {
    auto strongView = m_view.lock();
    if (!m_view.expired() && strongView->tag() != m_tag) {
      m_view.reset();
    }
    return strongView;
  }

 private:
  facebook::react::Tag m_tag;
  std::weak_ptr<IComponentView> m_view;
};

} // namespace Microsoft::ReactNative
