// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';

class AppThemeModule extends NativeEventEmitter  {
  get currentTheme(): string {
    return '';
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;