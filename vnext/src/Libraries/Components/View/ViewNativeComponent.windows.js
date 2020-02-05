/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const Platform = require('../../Utilities/Platform');
const ReactNative = require('../../Renderer/shims/ReactNative');
const ReactNativeViewViewConfigAndroid = require('./ReactNativeViewViewConfigAndroid');

const registerGeneratedViewConfig = require('../../Utilities/registerGeneratedViewConfig');
const requireNativeComponent = require('../../ReactNative/requireNativeComponent');

import type {ViewProps} from './ViewPropTypes';

export type ViewNativeComponentType = Class<
  ReactNative.NativeComponent<ViewProps>,
>;

let NativeViewComponent;
let viewConfig:
  | $TEMPORARY$object<{||}>
  | $TEMPORARY$object<{|
      bubblingEventTypes?: $ReadOnly<{
        [eventName: string]: $ReadOnly<{|
          phasedRegistrationNames: $ReadOnly<{|
            bubbled: string,
            captured: string,
          |}>,
        |}>,
      }>,
      directEventTypes?: $ReadOnly<{
        [eventName: string]: $ReadOnly<{|registrationName: string|}>,
      }>,
      uiViewClassName: string,
      validAttributes?: {
        [propName: string]:
          | true
          | $ReadOnly<{|
              diff?: <T>(arg1: any, arg2: any) => boolean,
              process?: (arg1: any) => any,
            |}>,
      },
    |}>;

// Only use the JS view config in DEV
// [Windows
// On Windows, always use the production code path.  The debug code path registers
// a ViewConfig that the Windows fork doesn't support.
//</T>if (__DEV__) {
if (false) {
  // Windows]
  // On Android, View extends the base component with additional view-only props
  // On iOS, the base component is View
  if (Platform.OS === 'android') {
    viewConfig = ReactNativeViewViewConfigAndroid;
    registerGeneratedViewConfig('RCTView', ReactNativeViewViewConfigAndroid);
  } else {
    viewConfig = {};
    registerGeneratedViewConfig('RCTView', {uiViewClassName: 'RCTView'});
  }

  NativeViewComponent = 'RCTView';
} else {
  NativeViewComponent = requireNativeComponent('RCTView');
}

export const __INTERNAL_VIEW_CONFIG = viewConfig;
export default ((NativeViewComponent: any): ViewNativeComponentType);
