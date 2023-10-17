/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow strict-local
 */
'use strict';

import typeof UnimplementedViewType from '../UnimplementedViews/UnimplementedView';
import typeof ProgressBarAndroidNativeComponentType from './ProgressBarAndroidNativeComponent';

module.exports = (require('../UnimplementedViews/UnimplementedView'):
  | UnimplementedViewType
  | ProgressBarAndroidNativeComponentType);
