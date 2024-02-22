/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow strict-local
 * @format
 */

import type {Double, DirectEventHandler, Int32} from '../../Types/CodegenTypes';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type {HostComponent} from 'react-native/Libraries/Renderer/shims/ReactNativeTypes';
import type {ViewProps} from '../View/ViewPropTypes';

type DismissEvent = $ReadOnly<{|
  isOpen?: boolean,
|}>;

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  horizontalOffset?: Double,
  isLightDismissEnabled?: boolean,
  autoFocus?: boolean,
  shouldConstrainToRootBounds?: boolean,

  /**
   * Specifies whether the area outside the flyout is darkened
   */
  isOverlayEnabled?: boolean,

  isOpen?: boolean,
  onDismiss?: ?DirectEventHandler<DismissEvent>,
  placement?: string,
  showMode?: string,
  target?: Int32,
  verticalOffset?: Double,

  // Events
|}>;

type NativeType = HostComponent<NativeProps>;

export default (codegenNativeComponent<NativeProps>('RCTFlyout', {
  interfaceOnly: true,
}): NativeType);
