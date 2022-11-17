/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow strict-local
 * @format
 */

import type {ColorValue} from '../../StyleSheet/StyleSheet';
import type {Double} from '../../Types/CodegenTypes';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type {HostComponent} from 'react-native/Libraries/Renderer/shims/ReactNativeTypes';
import type {ViewProps} from '../View/ViewPropTypes';

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  color?: ?ColorValue,
  emSize?: Double,
  fontUri: string,
  glyph: string,
  colorEnabled?: boolean,

  // Events
|}>;

type NativeType = HostComponent<NativeProps>;

export default (codegenNativeComponent<NativeProps>('PLYIcon', {
  interfaceOnly: true,
}): NativeType);
