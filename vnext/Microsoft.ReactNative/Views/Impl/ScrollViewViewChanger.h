// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Microsoft::ReactNative {

class ScrollViewViewChanger {
 public:
  void Horizontal(bool horizontal);
  void Inverted(bool inverted);

  void ScrollToEnd(const xaml::Controls::ScrollViewer &scrollViewer, bool animated);

  void OnViewChanging(
      const xaml::Controls::ScrollViewer &scrollViewer,
      const xaml::Controls::ScrollViewerViewChangingEventArgs &args);
  void OnViewChanged(
      const xaml::Controls::ScrollViewer &scrollViewer,
      const xaml::Controls::ScrollViewerViewChangedEventArgs &args);

 private:
  bool m_inverted{false};
  bool m_horizontal{false};
  bool m_wasScrollAnchoringEnabled{false};
  std::optional<double> m_targetScrollToEndOffset{std::nullopt};

  inline bool IsScrollToEndActive() {
    return m_targetScrollToEndOffset.has_value();
  }

  static void UpdateScrollAnchoringEnabled(const xaml::Controls::ScrollViewer &scrollViewer, bool enabled);
  bool
  IsScrollingToEnd(const xaml::Controls::ScrollViewer &scrollViewer, double horizontalOffset, double verticalOffset);
};

} // namespace Microsoft::ReactNative
