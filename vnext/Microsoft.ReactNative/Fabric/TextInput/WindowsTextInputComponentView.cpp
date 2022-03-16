
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <unicode.h>
#include "WindowsTextInputShadowNode.h"
#include "WindowsTextInputState.h"

namespace Microsoft::ReactNative {

facebook::react::AttributedString WindowsTextInputComponentView::getAttributedString() const {
  // Use BaseTextShadowNode to get attributed string from children

  auto childTextAttributes = facebook::react::TextAttributes::defaultTextAttributes();

  childTextAttributes.apply(m_props->textAttributes);

  auto attributedString = facebook::react::AttributedString{};
  // auto attachments = facebook::react::BaseTextShadowNode::Attachments{};

  // BaseTextShadowNode only gets children. We must detect and prepend text
  // value attributes manually.
  auto text = winrt::to_string(m_element.Text());
  if (!text.empty()) {
    auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
    textAttributes.apply(m_props->textAttributes);
    auto fragment = facebook::react::AttributedString::Fragment{};
    fragment.string = text;
    fragment.textAttributes = textAttributes;
    fragment.textAttributes.backgroundColor = facebook::react::clearColor();
    attributedString.prependFragment(fragment);
  }

  return attributedString;
}

WindowsTextInputComponentView::WindowsTextInputComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::WindowsTextInputProps const>();
  m_props = defaultProps;

  m_textChangedRevoker =
      m_element.TextChanged(winrt::auto_revoke, [this](auto sender, xaml::Controls::TextChangedEventArgs args) {
        auto data = m_state->getData();
        data.attributedString = getAttributedString();
        data.mostRecentEventCount = m_nativeEventCount;
        m_state->updateState(std::move(data));

        if (m_eventEmitter && !m_comingFromJS) {
          auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
          facebook::react::WindowsTextInputEventEmitter::OnChange onChangeArgs;
          onChangeArgs.text = winrt::to_string(m_element.Text());
          onChangeArgs.eventCount = ++m_nativeEventCount;
          emitter->onChange(onChangeArgs);
        }
      });

  m_SelectionChangedRevoker = m_element.SelectionChanged(winrt::auto_revoke, [this](auto sender, auto args) {
    if (m_eventEmitter) {
      auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
      facebook::react::WindowsTextInputEventEmitter::OnSelectionChange onSelectionChangeArgs;
      onSelectionChangeArgs.selection.start = m_element.SelectionStart();
      onSelectionChangeArgs.selection.end = m_element.SelectionStart() + m_element.SelectionLength();
      emitter->onSelectionChange(onSelectionChangeArgs);
    }
  });
}

void WindowsTextInputComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  if (commandName == "setTextAndSelection") {
    auto eventCount = arg[0].asInt();

    if (eventCount >= m_nativeEventCount) {
      auto text = arg[1].asString();
      auto begin = arg[2].asInt();
      auto end = arg[3].asInt();
      m_comingFromJS = true;
      SetText(winrt::to_hstring(text));

      if (!(begin < 0 || end < 0 || begin > end)) {
        m_element.Select(static_cast<int32_t>(begin), static_cast<int32_t>(end - begin));
      }
      m_comingFromJS = false;
    }
  } else {
    Super::handleCommand(commandName, arg);
  }
}

std::vector<facebook::react::ComponentDescriptorProvider>
WindowsTextInputComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void WindowsTextInputComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void WindowsTextInputComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void WindowsTextInputComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldTextInputProps = *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(m_props);
  const auto &newTextInputProps = *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(props);

  if (oldTextInputProps.textAttributes.foregroundColor != newTextInputProps.textAttributes.foregroundColor) {
    if (newTextInputProps.textAttributes.foregroundColor)
      m_element.Foreground(newTextInputProps.textAttributes.foregroundColor.AsWindowsBrush());
    else
      m_element.ClearValue(::xaml::Controls::TextBlock::ForegroundProperty());
  }

  if (oldTextInputProps.textAttributes.fontSize != newTextInputProps.textAttributes.fontSize) {
    if (std::isnan(newTextInputProps.textAttributes.fontSize)) {
      m_element.FontSize(facebook::react::TextAttributes::defaultTextAttributes().fontSize);
    } else {
      m_element.FontSize(newTextInputProps.textAttributes.fontSize);
    }
  }

  if (oldTextInputProps.textAttributes.fontWeight != newTextInputProps.textAttributes.fontWeight) {
    m_element.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(
        newTextInputProps.textAttributes.fontWeight.value_or(static_cast<facebook::react::FontWeight>(400)))});
  }

  if (oldTextInputProps.textAttributes.fontStyle != newTextInputProps.textAttributes.fontStyle) {
    switch (newTextInputProps.textAttributes.fontStyle.value_or(facebook::react::FontStyle::Normal)) {
      case facebook::react::FontStyle::Italic:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
        break;
      case facebook::react::FontStyle::Normal:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Normal);
        break;
      case facebook::react::FontStyle::Oblique:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Oblique);
        break;
      default:
        assert(false);
    }
  }

  if (oldTextInputProps.textAttributes.fontFamily != newTextInputProps.textAttributes.fontFamily) {
    if (newTextInputProps.textAttributes.fontFamily.empty())
      m_element.FontFamily(xaml::Media::FontFamily(L"Segoe UI"));
    else
      m_element.FontFamily(xaml::Media::FontFamily(
          Microsoft::Common::Unicode::Utf8ToUtf16(newTextInputProps.textAttributes.fontFamily)));
  }

  if (oldTextInputProps.allowFontScaling != newTextInputProps.allowFontScaling) {
    m_element.IsTextScaleFactorEnabled(newTextInputProps.allowFontScaling);
  }

  if (oldTextInputProps.maxLength != newTextInputProps.maxLength) {
    m_element.MaxLength(newTextInputProps.maxLength);
  }

  if (oldTextInputProps.placeholder != newTextInputProps.placeholder) {
    m_element.PlaceholderText(winrt::to_hstring(newTextInputProps.placeholder));
  }

  if (oldTextInputProps.editable != newTextInputProps.editable) {
    m_element.IsReadOnly(!newTextInputProps.editable);
  }

  /*
    if (oldTextInputProps.multiline != newTextInputProps.multiline) {
      m_element.TextWrapping(newTextInputProps.multiline ? xaml::TextWrapping::Wrap : xaml::TextWrapping::NoWrap);
      m_element.AcceptsReturn(newTextInputProps.multiline);
    }
    */

  if (oldTextInputProps.selection.start != newTextInputProps.selection.start ||
      oldTextInputProps.selection.end != newTextInputProps.selection.end) {
    m_element.Select(
        newTextInputProps.selection.start, newTextInputProps.selection.end - newTextInputProps.selection.start);
  }

  if (oldTextInputProps.autoCapitalize != newTextInputProps.autoCapitalize) {
    if (newTextInputProps.autoCapitalize == "characters") {
      m_element.CharacterCasing(xaml::Controls::CharacterCasing::Upper);
    } else { // anything else turns off autoCap (should be "None" but
             // we don't support "words"/"senetences" yet)
      m_element.CharacterCasing(xaml::Controls::CharacterCasing::Normal);
    }
  }

  if (oldTextInputProps.backgroundColor != newTextInputProps.backgroundColor) {
    if (newTextInputProps.backgroundColor) {
      m_element.Background(newTextInputProps.backgroundColor.AsWindowsBrush());
    } else {
      m_element.ClearValue(xaml::Controls::Control::BackgroundProperty());
    }
  }

  if (oldTextInputProps.borderColors != newTextInputProps.borderColors) {
    if (newTextInputProps.borderColors.all) {
      m_element.BorderBrush(newTextInputProps.borderColors.all->AsWindowsBrush());
    } else {
      m_element.ClearValue(xaml::Controls::Control::BorderBrushProperty());
    }
  }

  m_props = std::static_pointer_cast<facebook::react::WindowsTextInputProps const>(props);
}

void WindowsTextInputComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = std::static_pointer_cast<facebook::react::WindowsTextInputShadowNode::ConcreteState const>(state);

  if (!m_state) {
    assert(false && "State is `null` for <TextInput> component.");
    m_element.Text(L"");
    return;
  }

  auto data = m_state->getData();

  if (!oldState) {
    m_mostRecentEventCount = m_state->getData().mostRecentEventCount;
  }

  if (m_mostRecentEventCount == m_state->getData().mostRecentEventCount) {
    m_comingFromState = true;
    // Only handle single/empty fragments right now -- ignore the other fragments
    SetText(
        m_state->getData().attributedString.getFragments().size()
            ? winrt::to_hstring(m_state->getData().attributedString.getFragments()[0].string)
            : L"");
    m_comingFromState = false;
  }
}

void WindowsTextInputComponentView::SetText(winrt::hstring text) noexcept {
  auto oldCursor = m_element.SelectionStart();
  auto oldSelectionLength = m_element.SelectionLength();
  auto oldValue = m_element.Text();
  auto newValue = text;
  if (oldValue != newValue) {
    m_element.Text(newValue);
    if (oldValue.size() == newValue.size()) {
      m_element.SelectionStart(oldCursor);
    } else {
      m_element.SelectionStart(newValue.size());
    }
  }
}

void WindowsTextInputComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  m_element.BorderThickness(
      {m_layoutMetrics.borderWidth.left,
       m_layoutMetrics.borderWidth.top,
       m_layoutMetrics.borderWidth.right,
       m_layoutMetrics.borderWidth.bottom});

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.MinHeight(0);

  m_element.Padding({
      layoutMetrics.contentInsets.left - layoutMetrics.borderWidth.left,
      layoutMetrics.contentInsets.top - layoutMetrics.borderWidth.top,
      layoutMetrics.contentInsets.right - layoutMetrics.borderWidth.right,
      layoutMetrics.contentInsets.bottom - layoutMetrics.borderWidth.bottom,
  });

  m_element.ClearValue(xaml::FrameworkElement::HeightProperty());
  m_element.ClearValue(xaml::FrameworkElement::WidthProperty());
  m_element.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
  auto ds = m_element.DesiredSize();

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void WindowsTextInputComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void WindowsTextInputComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps WindowsTextInputComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement WindowsTextInputComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
