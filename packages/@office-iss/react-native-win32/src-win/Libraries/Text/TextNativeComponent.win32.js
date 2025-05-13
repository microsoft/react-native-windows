/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

import type {HostComponent} from '../Renderer/shims/ReactNativeTypes';
import type {ProcessedColorValue} from '../StyleSheet/processColor';
import type {GestureResponderEvent} from '../Types/CoreEventTypes';
import type {TextProps} from './TextProps';

import {createViewConfig} from '../NativeComponent/ViewConfig';
import UIManager from '../ReactNative/UIManager';
import createReactNativeComponentClass from '../Renderer/shims/createReactNativeComponentClass';

export type NativeTextProps = $ReadOnly<{
  ...TextProps,
  isHighlighted?: ?boolean,
  selectionColor?: ?ProcessedColorValue,
  onClick?: ?(event: GestureResponderEvent) => mixed,
  // This is only needed for platforms that optimize text hit testing, e.g.,
  // react-native-windows. It can be used to only hit test virtual text spans
  // that have pressable events attached to them.
  isPressable?: ?boolean,
}>;

const textViewConfig = {
  validAttributes: {
    isHighlighted: true,
    isPressable: true,
    numberOfLines: true,
    ellipsizeMode: true,
    allowFontScaling: true,
    dynamicTypeRamp: true,
    maxFontSizeMultiplier: true,
    disabled: true,
    selectable: true,
    selectionColor: true,
    adjustsFontSizeToFit: true,
    minimumFontScale: true,
    textBreakStrategy: true,
    onTextLayout: true,
    onInlineViewLayout: true,
    dataDetectorType: true,
    android_hyphenationFrequency: true,
    lineBreakStrategyIOS: true,
    // [Windows
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
    textStyle: true,
    tooltip: true,
    // Windows]
  },
  directEventTypes: {
    topTextLayout: {
      registrationName: 'onTextLayout',
    },
    topInlineViewLayout: {
      registrationName: 'onInlineViewLayout',
    },
  },
  uiViewClassName: 'RCTText',
};

const virtualTextViewConfig = {
  validAttributes: {
    isHighlighted: true,
    isPressable: true,
    maxFontSizeMultiplier: true,
  },
  uiViewClassName: 'RCTVirtualText',
};

export const NativeText: HostComponent<NativeTextProps> =
  (createReactNativeComponentClass('RCTText', () =>
    createViewConfig(textViewConfig),
  ): any);

export const NativeVirtualText: HostComponent<NativeTextProps> =
  !global.RN$Bridgeless &&
  UIManager.getViewManagerConfig('RCTVirtualText') == null
    ? NativeText
    : (createReactNativeComponentClass('RCTVirtualText', () =>
        createViewConfig(virtualTextViewConfig),
      ): any);
