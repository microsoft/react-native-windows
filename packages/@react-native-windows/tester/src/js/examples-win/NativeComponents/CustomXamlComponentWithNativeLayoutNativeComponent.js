/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

import type {ViewProps} from 'react-native/Libraries/Components/View/ViewPropTypes';
import type {HostComponent} from '../../../src/private/types/HostComponent';
import type {DirectEventHandler} from 'react-native/Libraries/Types/CodegenTypes';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';

type MyEventEvent = $ReadOnly<{|
  value: boolean,
  target: Int32,
|}>;

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  label: string,

  // Events
  onMyEvent?: ?DirectEventHandler<MyEventEvent>,
|}>;

type ComponentType = HostComponent<NativeProps>;

export default (codegenNativeComponent<NativeProps>(
  'CustomXamlComponentWithNativeLayout',
): ComponentType);
