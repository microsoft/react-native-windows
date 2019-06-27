// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';

class AppThemeModule extends NativeEventEmitter  {
  get currentTheme(): string {
    return 'light';
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;