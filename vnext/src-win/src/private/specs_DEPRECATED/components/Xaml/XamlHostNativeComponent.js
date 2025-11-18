/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

import type {ViewProps} from 'react-native';
import {codegenNativeComponent} from 'react-native';
import type {HostComponent} from '../../../../../src/private/types/HostComponent';

type XamlHostProps = $ReadOnly<{|
  ...ViewProps,
|}>;

type NativeType = HostComponent<XamlHostProps>;

export default (codegenNativeComponent<XamlHostProps>('XamlHost'): NativeType);
