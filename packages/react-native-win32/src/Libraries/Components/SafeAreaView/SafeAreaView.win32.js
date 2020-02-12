/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

const Platform = require('../../Utilities/Platform');
const React = require('react');
const View = require('../View/View');

import type {NativeComponent} from '../../Renderer/shims/ReactNative';
import type {ViewProps} from '../View/ViewPropTypes';

type Props = $ReadOnly<{|
  ...ViewProps,
  emulateUnlessSupported?: boolean,
|}>;

let exported: Class<React$Component<Props>> | Class<NativeComponent<Props>>;

/**
 * Renders nested content and automatically applies paddings reflect the portion
 * of the view that is not covered by navigation bars, tab bars, toolbars, and
 * other ancestor views.
 *
 * Moreover, and most importantly, Safe Area's paddings reflect physical
 * limitation of the screen, such as rounded corners or camera notches (aka
 * sensor housing area on iPhone X).
 */
// [Win32 - Added win32 to if
if (Platform.OS === 'android' || Platform.OS === 'win32') {
  // Win32]
  const SafeAreaView = (
    props: Props,
    forwardedRef?: ?React.Ref<typeof View>,
  ) => {
    const {emulateUnlessSupported, ...localProps} = props;
    return <View {...localProps} ref={forwardedRef} />;
  };

  const SafeAreaViewRef = React.forwardRef(SafeAreaView);
  SafeAreaViewRef.displayName = 'SafeAreaView';
  exported = ((SafeAreaViewRef: any): Class<React.Component<Props>>);
} else {
  const RCTSafeAreaViewNativeComponent = require('./RCTSafeAreaViewNativeComponent')
    .default;

  const SafeAreaView = (
    props: Props,
    forwardedRef?: ?React.Ref<typeof RCTSafeAreaViewNativeComponent>,
  ) => {
    return (
      <RCTSafeAreaViewNativeComponent
        emulateUnlessSupported={true}
        {...props}
        ref={forwardedRef}
      />
    );
  };

  const SafeAreaViewRef = React.forwardRef(SafeAreaView);
  SafeAreaViewRef.displayName = 'SafeAreaView';
  exported = ((SafeAreaViewRef: any): Class<NativeComponent<Props>>);
}

module.exports = exported;
