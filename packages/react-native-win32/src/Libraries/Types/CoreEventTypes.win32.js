/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import * as React from 'react';
import type {HostComponent} from '../Renderer/shims/ReactNativeTypes';

export type SyntheticEvent<T> = $ReadOnly<{|
  bubbles: ?boolean,
  cancelable: ?boolean,
  currentTarget: number | React.ElementRef<HostComponent<mixed>>,
  defaultPrevented: ?boolean,
  dispatchConfig: $ReadOnly<{|
    registrationName: string,
  |}>,
  eventPhase: ?number,
  preventDefault: () => void,
  isDefaultPrevented: () => boolean,
  stopPropagation: () => void,
  isPropagationStopped: () => boolean,
  isTrusted: ?boolean,
  nativeEvent: T,
  persist: () => void,
  target: ?number | React.ElementRef<HostComponent<mixed>>,
  timeStamp: number,
  type: ?string,
|}>;

export type ResponderSyntheticEvent<T> = $ReadOnly<{|
  ...SyntheticEvent<T>,
  touchHistory: $ReadOnly<{|
    indexOfSingleActiveTouch: number,
    mostRecentTimeStamp: number,
    numberActiveTouches: number,
    touchBank: $ReadOnlyArray<
      $ReadOnly<{|
        touchActive: boolean,
        startPageX: number,
        startPageY: number,
        startTimeStamp: number,
        currentPageX: number,
        currentPageY: number,
        currentTimeStamp: number,
        previousPageX: number,
        previousPageY: number,
        previousTimeStamp: number,
      |}>,
    >,
  |}>,
|}>;

export type Layout = $ReadOnly<{|
  x: number,
  y: number,
  width: number,
  height: number,
|}>;

export type TextLayout = $ReadOnly<{|
  ...Layout,
  ascender: number,
  capHeight: number,
  descender: number,
  text: string,
  xHeight: number,
|}>;

export type LayoutEvent = SyntheticEvent<
  $ReadOnly<{|
    layout: Layout,
  |}>,
>;

export type TextLayoutEvent = SyntheticEvent<
  $ReadOnly<{|
    lines: Array<TextLayout>,
  |}>,
>;

export type PressEvent = ResponderSyntheticEvent<
  $ReadOnly<{|
    altKey: ?boolean, // TODO(macOS)
    button: ?number, // TODO(macOS)
    changedTouches: $ReadOnlyArray<$PropertyType<PressEvent, 'nativeEvent'>>,
    ctrlKey: ?boolean, // TODO(macOS)
    force?: number,
    identifier: number,
    locationX: number,
    locationY: number,
    metaKey: ?boolean, // TODO(macOS)
    pageX: number,
    pageY: number,
    shiftKey: ?boolean, // TODO(macOS)
    target: ?number,
    timestamp: number,
    touches: $ReadOnlyArray<$PropertyType<PressEvent, 'nativeEvent'>>,
  |}>,
>;

export type ScrollEvent = SyntheticEvent<
  $ReadOnly<{|
    contentInset: $ReadOnly<{|
      bottom: number,
      left: number,
      right: number,
      top: number,
    |}>,
    contentOffset: $ReadOnly<{|
      y: number,
      x: number,
    |}>,
    contentSize: $ReadOnly<{|
      height: number,
      width: number,
    |}>,
    layoutMeasurement: $ReadOnly<{|
      height: number,
      width: number,
    |}>,
    targetContentOffset?: $ReadOnly<{|
      y: number,
      x: number,
    |}>,
    velocity?: $ReadOnly<{|
      y: number,
      x: number,
    |}>,
    zoomScale?: number,
    responderIgnoreScroll?: boolean,
    key?: string, // TODO(macOS)
  |}>,
>;

export type BlurEvent = SyntheticEvent<
  $ReadOnly<{|
    target: number,
  |}>,
>;

export type FocusEvent = SyntheticEvent<
  $ReadOnly<{|
    target: number,
  |}>,
>;

// [Windows Mouse events on Windows don't match up with the version in core
// introduced for react-native-web. Replace typings with our values to catch
// anything dependent on react-native-web specific values
export type MouseEvent = SyntheticEvent<
  $ReadOnly<{|
    target: number,
    identifier: number,
    pageX: number,
    pageY: number,
    locationX: number,
    locationY: number,
    timestamp: number,
    pointerType: string,
    force: number,
    isLeftButton: boolean,
    isRightButton: boolean,
    isMiddleButton: boolean,
    isBarrelButtonPressed: boolean,
    isHorizontalScrollWheel: boolean,
    isEraser: boolean,
    shiftKey: boolean,
    ctrlKey: boolean,
    altKey: boolean,
  |}>,
>;
// Windows]

// [Windows
export type KeyEvent = SyntheticEvent<
  $ReadOnly<{|
    altKey: boolean,
    ctrlKey: boolean,
    metaKey: boolean,
    shiftKey: boolean,
    key: string,
    code: string,
    eventPhase: number,
  |}>,
>;
// Windows]
