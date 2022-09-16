// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputProps.h"

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
      autoFocus(convertRawProp(context, rawProps, "autoFocus", sourceProps.autoFocus, {false})) {}

} // namespace facebook::react
