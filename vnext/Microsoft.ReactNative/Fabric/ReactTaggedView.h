
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
  ReactTaggedView(const winrt::Microsoft::ReactNative::ComponentView &componentView)
      : m_view(componentView), m_tag(componentView ? componentView.Tag() : -1) {}

  winrt::Microsoft::ReactNative::ComponentView view() noexcept {
    if (!m_view) {
      return nullptr;
    }

    auto strongView = m_view.get();
    if (!strongView || strongView.Tag() != m_tag) {
      m_view = nullptr;
    }

    return strongView;
  }

  facebook::react::Tag Tag() const noexcept {
    return m_tag;
  }

 private:
  facebook::react::Tag m_tag;
  winrt::weak_ref<winrt::Microsoft::ReactNative::ComponentView> m_view;
};

} // namespace Microsoft::ReactNative
