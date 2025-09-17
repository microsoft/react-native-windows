/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';

type XamlHostProps = $ReadOnly<{|
  ...ViewProps,

  // Props
  label?: string,
|}>;

export default codegenNativeComponent<XamlHostProps>('XamlHost');
