/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

// Temporary test example for UseExperimentalWinUI3=true
// Remove when we get react-native-xaml working well for Fabric

import type {HostComponent} from 'react-native/Libraries/Renderer/shims/ReactNativeTypes';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';

type SelectedDatesChangedEvent = $ReadOnly<{|
  value: boolean,
  target: Int32,
  startDate: string,
|}>;

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  label: string,

  // Events
  onSelectedDatesChanged?: ?DirectEventHandler<SelectedDatesChangedEvent>,
|}>;

type ComponentType = HostComponent<NativeProps>;

export default (codegenNativeComponent<NativeProps>(
  'CalendarView',
): ComponentType);
