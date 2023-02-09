/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow strict-local
 * @format
 */

import type {ColorValue} from '../../StyleSheet/StyleSheet';
import type {Double, DirectEventHandler, Int32} from '../../Types/CodegenTypes';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type {HostComponent} from 'react-native/Libraries/Renderer/shims/ReactNativeTypes';
import type {ViewProps} from '../View/ViewPropTypes';

type DismissEvent = $ReadOnly<{|
  isOpen?: boolean,
|}>;

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  isOpen?: boolean,

  /**
   * Configures the Popup with a transparent backdrop.
   */
  isLightDismissEnabled?: boolean,

  /**
   * Sets whether the Popup will automatically receive focus when opened.
   * Defaults to true. Always true when isLightDismissEnabled === true.
   */
  autoFocus?: boolean,

  horizontalOffset?: Double,
  verticalOffset?: Double,

  /**
   * Sets a React element to use as an anchor point. When set, the popup will be positioned relative to it.
   */
  target?: Int32,
  onDismiss?: ?DirectEventHandler<DismissEvent>,
  testID?: string,
|}>;

type NativeType = HostComponent<NativeProps>;

export default (codegenNativeComponent<NativeProps>('RCTPopup', {
  interfaceOnly: true,
}): NativeType);
