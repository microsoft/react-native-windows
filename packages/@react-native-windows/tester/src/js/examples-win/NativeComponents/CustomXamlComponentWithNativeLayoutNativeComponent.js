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

import {get} from 'react-native/Libraries/NativeComponent/NativeComponentRegistry';

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  label: string,
|}>;

// Cannot just use codegenNativeComponent, or registerNativeComponent, since we need to provide a custom config
const CustomXamlComponentWithNativeLayout = get<NativeProps>('CustomXamlComponentWithNativeLayout', () => {
  return {
    uiViewClassName: 'CustomXamlComponentWithNativeLayout',
    bubblingEventTypes: {},
    directEventTypes: {},
    validAttributes: {
      label: true,
    },
  };
});

exports.CustomXamlComponentWithNativeLayout = CustomXamlComponentWithNativeLayout;
