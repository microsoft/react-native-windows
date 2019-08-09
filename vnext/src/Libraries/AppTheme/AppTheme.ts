/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

const MissingNativeEventEmitterShim = require('MissingNativeEventEmitterShim');
import {IHighContrastColors} from './AppThemeTypes';

class AppThemeModule extends MissingNativeEventEmitterShim {
  public isAvailable = false;
  public currentTheme: 'light' | 'dark' = 'light';
  public isHighContrast = false;
  public currentHighContrastColors = {} as IHighContrastColors;
}

export const AppTheme = new AppThemeModule();
export default AppTheme;
