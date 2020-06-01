// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "TestHook.h"
#include <cdebug.h>

#ifdef DEBUG

// Obtained from https://reactnative.dev/docs/layout-props
// See getLayoutProps.ps1 next to this file.
std::vector<std::string> TestHook::layoutProperties = {
    "alignContent",
    "alignItems",
    "alignSelf",
    "aspectRatio",
    "borderBottomWidth",
    "borderEndWidth",
    "borderLeftWidth",
    "borderRightWidth",
    "borderStartWidth",
    "borderTopWidth",
    "borderWidth",
    "bottom",
    "direction",
    "display",
    "end",
    "flex",
    "flexBasis",
    "flexDirection",
    "flexGrow",
    "flexShrink",
    "flexWrap",
    "height",
    "justifyContent",
    "left",
    "margin",
    "marginBottom",
    "marginEnd",
    "marginHorizontal",
    "marginLeft",
    "marginRight",
    "marginStart",
    "marginTop",
    "marginVertical",
    "maxHeight",
    "maxWidth",
    "minHeight",
    "minWidth",
    "overflow",
    "padding",
    "paddingBottom",
    "paddingEnd",
    "paddingHorizontal",
    "paddingLeft",
    "paddingRight",
    "paddingStart",
    "paddingTop",
    "paddingVertical",
    "position",
    "right",
    "start",
    "top",
    "width",
    "zIndex",
};

void TestHook::NotifyUnimplementedProperty(
    const std::string &viewManager,
    const std::string &reactClassName,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  if (std::find(layoutProperties.begin(), layoutProperties.end(), propertyName) != layoutProperties.end()) {
    return;
  }
  std::string value;
  size_t size{};
  try {
    if (propertyValue.isObject()) {
      value = "[Object]";
    } else if (propertyValue.isNull()) {
      value = "[Null]";
    } else if (propertyValue.isArray()) {
      size = propertyValue.size();
      value = "[Array]";
    } else {
      value = propertyValue.asString();
    }
  } catch (const folly::TypeError &e) {
    value = e.what();
  }

  cdebug << "[UnimplementedProperty] ViewManager = " << viewManager << " elementClass = " << reactClassName
         << " propertyName = " << propertyName << " value = " << value;

  if (size != 0) {
    cdebug << " (" << size << " elems)";
  }

  cdebug << std::endl;
  // DebugBreak();
}

#endif // DEBUG
