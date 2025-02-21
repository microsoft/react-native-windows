/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type {ViewProps} from 'react-native';

export interface XamlHostProps extends ViewProps {
  label: string;
}

export default codegenNativeComponent<XamlHostProps>('XamlHost');
