/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow strict-local
 * @format
 */

import type {ViewProps} from '../View/ViewPropTypes';
import type {HostComponent} from '../../../src/private/types/HostComponent';
import type {
  DirectEventHandler,
  Double,
  Int32,
} from 'react-native/Libraries/Types/CodegenTypes';

import {codegenNativeComponent} from 'react-native';

type DismissEvent = $ReadOnly<{
  isOpen?: boolean,
}>;

type NativeProps = $ReadOnly<{
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
}>;

type NativeType = HostComponent<NativeProps>;
// $FlowFixMe[nonpolymorphic-type-arg]
export default (codegenNativeComponent<NativeProps>('RCTFlyout', {
  interfaceOnly: true,
}): NativeType);
