// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';
import { IHighContrastColors } from './AppThemeTypes';

class AppThemeModule extends NativeEventEmitter  {
  get currentTheme(): string {
    return '';
  }

  get isHighContrast(): boolean {
    return false;
  }

  get currentHighContrastColorValues(): IHighContrastColors {
    return { } as IHighContrastColors;
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;
