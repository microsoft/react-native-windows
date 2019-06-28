// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';
import { IColorValues } from '../AppThemeTypes';

class AppThemeModule extends NativeEventEmitter  {
  get currentTheme(): string {
    return 'light';
  }

  get isHighContrast(): boolean {
    return false;
  }

  get currentHighContrastColorValues(): IColorValues {
    return { } as IColorValues;
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;