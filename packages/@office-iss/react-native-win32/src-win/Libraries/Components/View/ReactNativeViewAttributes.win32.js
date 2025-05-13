/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';
import ReactNativeStyleAttributes from './ReactNativeStyleAttributes';

const UIView = {
  pointerEvents: true,
  accessible: true,
  accessibilityActions: true,
  accessibilityLabel: true,
  accessibilityLiveRegion: true,
  accessibilityRole: true,
  accessibilityState: true,
  accessibilityValue: true,
  accessibilityHint: true,
  accessibilityLanguage: true,
  accessibilityShowsLargeContentViewer: true,
  accessibilityLargeContentTitle: true,
  importantForAccessibility: true,
  nativeID: true,
  testID: true,
  renderToHardwareTextureAndroid: true,
  shouldRasterizeIOS: true,
  onLayout: true,
  onAccessibilityAction: true,
  onAccessibilityTap: true,
  onMagicTap: true,
  onAccessibilityEscape: true,
  collapsable: true,
  collapsableChildren: true,
  needsOffscreenAlphaCompositing: true,
  style: ReactNativeStyleAttributes,
  role: true,
  // [Win32
  accessibilityAccessKey: true,
  accessibilityAnnotation: true,
  accessibilityControls: true,
  accessibilityDescribedBy: true,
  accessibilityDescription: true,
  accessibilityItemType: true,
  accessibilityLevel: true,
  accessibilityPositionInSet: true,
  accessibilitySetSize: true,
  animationClass: true,
  cursor: true,
  draggedTypes: true,
  enableFocusRing: true,
  focusable: true,
  keyDownEvents: true,
  keyUpEvents: true,
  onBlur: true,
  onBlurCapture: true,
  onDragEnter: true,
  onDragLeave: true,
  onDrop: true,
  onFocus: true,
  onFocusCapture: true,
  onKeyDown: true,
  onKeyDownCapture: true,
  onKeyUp: true,
  onKeyUpCapture: true,
  onMouseEnter: true,
  onMouseLeave: true,
  tabIndex: true,
  textStyle: true, // Once we flush out our JS theming story this property will no longer be needed
  tooltip: true,
  // Win32]
};

const RCTView = {
  ...UIView,

  // This is a special performance property exposed by RCTView and useful for
  // scrolling content when there are many subviews, most of which are offscreen.
  // For this property to be effective, it must be applied to a view that contains
  // many subviews that extend outside its bound. The subviews must also have
  // overflow: hidden, as should the containing view (or one of its superviews).
  removeClippedSubviews: true,
};

const ReactNativeViewAttributes = {
  UIView: UIView,
  RCTView: RCTView,
};

export default ReactNativeViewAttributes;
