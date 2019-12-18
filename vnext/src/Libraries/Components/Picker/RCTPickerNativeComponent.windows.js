/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */
'use strict';

const requireNativeComponent = require('../../ReactNative/requireNativeComponent');

import type {SyntheticEvent} from '../../Types/CoreEventTypes';
import type {TextStyleProp} from '../../StyleSheet/StyleSheet';
import type {NativeComponent} from '../../Renderer/shims/ReactNative';
import type {NativeOrDynamicColorType} from '../../Color/NativeOrDynamicColorType'; // ]TODO(macOS ISS#2323203)

type PickerIOSChangeEvent = SyntheticEvent<
  $ReadOnly<{|
    newValue: number | string,
    newIndex: number,
  |}>,
>;

type RCTPickerIOSItemType = $ReadOnly<{|
  label: ?Label,
  value: ?(number | string),
  textColor: ?(number | NativeOrDynamicColorType), // TODO(macOS ISS#2323203)
|}>;

type Label = Stringish | number;

type RCTPickerIOSType = Class<
  NativeComponent<
    $ReadOnly<{|
      items: $ReadOnlyArray<RCTPickerIOSItemType>,
      onChange: (event: PickerIOSChangeEvent) => void,
      onResponderTerminationRequest: () => boolean,
      onStartShouldSetResponder: () => boolean,
      selectedIndex: number,
      style?: ?TextStyleProp,
      testID?: ?string,
    |}>,
  >,
>;

module.exports = ((requireNativeComponent('RCTPicker'): any): RCTPickerIOSType);
