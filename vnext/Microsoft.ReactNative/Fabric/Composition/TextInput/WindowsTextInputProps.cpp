// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputProps.h"
#include <react/renderer/core/PropsMacros.h>

namespace facebook::react {

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
      autoCorrect(convertRawProp(context, rawProps, "autoCorrect", sourceProps.autoCorrect, {true})),
      clearTextOnFocus(convertRawProp(context, rawProps, "clearTextOnFocus", sourceProps.clearTextOnFocus, {false})),
      editable(convertRawProp(context, rawProps, "editable", sourceProps.editable, {true})),
      maxLength(convertRawProp(context, rawProps, "maxLength", sourceProps.maxLength, {0})),
      multiline(convertRawProp(context, rawProps, "multiline", sourceProps.multiline, {false})),
      placeholder(convertRawProp(context, rawProps, "placeholder", sourceProps.placeholder, {})),
      placeholderTextColor(
          convertRawProp(context, rawProps, "placeholderTextColor", sourceProps.placeholderTextColor, {})),
      scrollEnabled(convertRawProp(context, rawProps, "scrollEnabled", sourceProps.scrollEnabled, {true})),
      cursorColor(convertRawProp(context, rawProps, "cursorColor", sourceProps.cursorColor, {})),
      selection(convertRawProp(context, rawProps, "selection", sourceProps.selection, {})),
      selectionColor(convertRawProp(context, rawProps, "selectionColor", sourceProps.selectionColor, {})),
      selectTextOnFocus(convertRawProp(context, rawProps, "selectTextOnFocus", sourceProps.selectTextOnFocus, {false})),
      spellCheck(convertRawProp(context, rawProps, "spellCheck", sourceProps.spellCheck, {true})),
      text(convertRawProp(context, rawProps, "text", sourceProps.text, {})),
      mostRecentEventCount(
          convertRawProp(context, rawProps, "mostRecentEventCount", sourceProps.mostRecentEventCount, {0})),
      secureTextEntry(convertRawProp(context, rawProps, "secureTextEntry", sourceProps.secureTextEntry, {false})),
      keyboardType(convertRawProp(context, rawProps, "keyboardType", sourceProps.keyboardType, {})),
      contextMenuHidden(convertRawProp(context, rawProps, "contextMenuHidden", sourceProps.contextMenuHidden, {false})),
      caretHidden(convertRawProp(context, rawProps, "caretHidden", sourceProps.caretHidden, {false})),
      autoCapitalize(convertRawProp(context, rawProps, "autoCapitalize", sourceProps.autoCapitalize, {})),
      clearTextOnSubmit(convertRawProp(context, rawProps, "clearTextOnSubmit", sourceProps.clearTextOnSubmit, {false})),
      submitBehavior(convertRawProp(context, rawProps, "submitBehavior", sourceProps.submitBehavior, {})),
      submitKeyEvents(convertRawProp(context, rawProps, "submitKeyEvents", sourceProps.submitKeyEvents, {})),
      autoFocus(convertRawProp(context, rawProps, "autoFocus", sourceProps.autoFocus, {false})),
      textAlign(
          convertRawProp(context, rawProps, "textAlign", sourceProps.textAlign, facebook::react::TextAlignment::Left)) {
}

void WindowsTextInputProps::setProp(
    const PropsParserContext &context,
    RawPropsPropNameHash hash,
    const char *propName,
    RawValue const &value) {
  BaseTextProps::setProp(context, hash, propName, value);
  ViewProps::setProp(context, hash, propName, value);

  static auto defaults = WindowsTextInputProps{};

  switch (hash) {
    RAW_SET_PROP_SWITCH_CASE_BASIC(allowFontScaling);
    RAW_SET_PROP_SWITCH_CASE_BASIC(autoCorrect);
    RAW_SET_PROP_SWITCH_CASE_BASIC(clearTextOnFocus);
    RAW_SET_PROP_SWITCH_CASE_BASIC(editable);
    RAW_SET_PROP_SWITCH_CASE_BASIC(maxLength);
    RAW_SET_PROP_SWITCH_CASE_BASIC(multiline);
    RAW_SET_PROP_SWITCH_CASE_BASIC(placeholder);
    RAW_SET_PROP_SWITCH_CASE_BASIC(placeholderTextColor);
    RAW_SET_PROP_SWITCH_CASE_BASIC(scrollEnabled);
    RAW_SET_PROP_SWITCH_CASE_BASIC(cursorColor);
    RAW_SET_PROP_SWITCH_CASE_BASIC(selection);
    RAW_SET_PROP_SWITCH_CASE_BASIC(selectionColor);
    RAW_SET_PROP_SWITCH_CASE_BASIC(selectTextOnFocus);
    RAW_SET_PROP_SWITCH_CASE_BASIC(spellCheck);
    RAW_SET_PROP_SWITCH_CASE_BASIC(text);
    RAW_SET_PROP_SWITCH_CASE_BASIC(mostRecentEventCount);
    RAW_SET_PROP_SWITCH_CASE_BASIC(secureTextEntry);
    RAW_SET_PROP_SWITCH_CASE_BASIC(keyboardType);
    RAW_SET_PROP_SWITCH_CASE_BASIC(contextMenuHidden);
    RAW_SET_PROP_SWITCH_CASE_BASIC(caretHidden);
    RAW_SET_PROP_SWITCH_CASE_BASIC(autoCapitalize);
    RAW_SET_PROP_SWITCH_CASE_BASIC(clearTextOnSubmit);
    RAW_SET_PROP_SWITCH_CASE_BASIC(submitBehavior);
    RAW_SET_PROP_SWITCH_CASE_BASIC(submitKeyEvents);
    RAW_SET_PROP_SWITCH_CASE_BASIC(autoFocus);
    RAW_SET_PROP_SWITCH_CASE_BASIC(textAlign);
  }
}

} // namespace facebook::react
