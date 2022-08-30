// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputProps.h"

namespace facebook::react {

static bool
hasValue(const RawProps &rawProps, bool defaultValue, const char *name, const char *prefix, const char *suffix) {
  auto rawValue = rawProps.at(name, prefix, suffix);

  // No change to prop - use default
  if (rawValue == nullptr) {
    return defaultValue;
  }

  // Value passed from JS
  if (rawValue->hasValue()) {
    return true;
  }

  // Null/undefined passed in, indicating that we should use the default
  // platform value - thereby resetting this
  return false;
}

WindowsTextInputProps::WindowsTextInputProps(
    const PropsParserContext &context,
    const WindowsTextInputProps &sourceProps,
    const RawProps &rawProps)
    : ViewProps(context, sourceProps, rawProps),
      BaseTextProps(context, sourceProps, rawProps),

      /*
      traits(convertRawProp(context, rawProps, sourceProps.traits, {})),
      paragraphAttributes(convertRawProp(
          context,
          rawProps,
          sourceProps.paragraphAttributes,
          {})),
          */

      allowFontScaling(convertRawProp(context, rawProps, "allowFontScaling", sourceProps.allowFontScaling, {true})),
      clearTextOnFocus(convertRawProp(context, rawProps, "clearTextOnFocus", sourceProps.clearTextOnFocus, {false})),
      editable(convertRawProp(context, rawProps, "editable", sourceProps.editable, {true})),
      maxLength(convertRawProp(context, rawProps, "maxLength", sourceProps.maxLength, {0})),
      multiline(convertRawProp(context, rawProps, "multiline", sourceProps.multiline, {false})),
      placeholder(convertRawProp(context, rawProps, "placeholder", sourceProps.placeholder, {})),
      placeholderTextColor(
          convertRawProp(context, rawProps, "placeholderTextColor", sourceProps.placeholderTextColor, {})),
      scrollEnabled(convertRawProp(context, rawProps, "scrollEnabled", sourceProps.scrollEnabled, {true})),
      selection(convertRawProp(context, rawProps, "selection", sourceProps.selection, {})),
      selectionColor(convertRawProp(context, rawProps, "selectionColor", sourceProps.selectionColor, {})),
      selectTextOnFocus(convertRawProp(context, rawProps, "selectTextOnFocus", sourceProps.selectTextOnFocus, {false})),
      spellCheck(convertRawProp(context, rawProps, "spellCheck", sourceProps.spellCheck, {false})),
      text(convertRawProp(context, rawProps, "text", sourceProps.text, {})),
      mostRecentEventCount(
          convertRawProp(context, rawProps, "mostRecentEventCount", sourceProps.mostRecentEventCount, {0})),
      secureTextEntry(convertRawProp(context, rawProps, "secureTextEntry", sourceProps.secureTextEntry, {false})),
      keyboardType(convertRawProp(context, rawProps, "keyboardType", sourceProps.keyboardType, {})),
      contextMenuHidden(convertRawProp(context, rawProps, "contextMenuHidden", sourceProps.contextMenuHidden, {false})),
      caretHidden(convertRawProp(context, rawProps, "caretHidden", sourceProps.caretHidden, {false})),
      autoCapitalize(convertRawProp(context, rawProps, "autoCapitalize", sourceProps.autoCapitalize, {})),
      clearTextOnSubmit(convertRawProp(context, rawProps, "clearTextOnSubmit", sourceProps.clearTextOnSubmit, {false})),
      submitKeyEvents(convertRawProp(context, rawProps, "submitKeyEvents", sourceProps.submitKeyEvents, {})),
      autoFocus(convertRawProp(context, rawProps, "autoFocus", sourceProps.autoFocus, {false})),
      // See AndroidTextInputComponentDescriptor for usage
      // TODO T63008435: can these, and this feature, be removed entirely?
      hasPadding(hasValue(rawProps, sourceProps.hasPadding, "", "padding", "")),
      hasPaddingHorizontal(hasValue(rawProps, sourceProps.hasPaddingHorizontal, "Horizontal", "padding", "")),
      hasPaddingVertical(hasValue(rawProps, sourceProps.hasPaddingVertical, "Vertical", "padding", "")),
      hasPaddingLeft(hasValue(rawProps, sourceProps.hasPaddingLeft, "Left", "padding", "")),
      hasPaddingTop(hasValue(rawProps, sourceProps.hasPaddingTop, "Top", "padding", "")),
      hasPaddingRight(hasValue(rawProps, sourceProps.hasPaddingRight, "Right", "padding", "")),
      hasPaddingBottom(hasValue(rawProps, sourceProps.hasPaddingBottom, "Bottom", "padding", "")),
      hasPaddingStart(hasValue(rawProps, sourceProps.hasPaddingStart, "Start", "padding", "")),
      hasPaddingEnd(hasValue(rawProps, sourceProps.hasPaddingEnd, "End", "padding", "")) {}

} // namespace facebook::react
