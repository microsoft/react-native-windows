// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <stack>

namespace Microsoft::ReactNative {

template <class T>
struct TextVisitorScope {
  TextVisitorScope(std::stack<T> &stack, T scopedValue) : m_stack{stack} {
    // There's an assumption here that the scoped value should not be pushed if
    // it has a default value. Check this assumption for future usages.
    if (scopedValue != T{}) {
      m_stack.push(scopedValue);
      m_pushed = true;
    }
  }

  ~TextVisitorScope() {
    if (m_pushed) {
      m_stack.pop();
    }
  }

  TextVisitorScope(TextVisitorScope const &) = delete;
  TextVisitorScope &operator=(TextVisitorScope other) = delete;

 private:
  std::stack<T> &m_stack;
  bool m_pushed{false};
};

} // namespace Microsoft::ReactNative
