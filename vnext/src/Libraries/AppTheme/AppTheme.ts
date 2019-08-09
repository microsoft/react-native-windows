/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

const MissingNativeEventEmitterShim = require('MissingNativeEventEmitterShim');
import {AppThemeTypes, IHighContrastColors} from './AppThemeTypes';

class AppThemeModule extends MissingNativeEventEmitterShim {
  public isAvailable = false;
  public currentTheme: AppThemeTypes = 'light';
  public isHighContrast = false;
  public currentHighContrastColors = {} as IHighContrastColors;
}

export const AppTheme = new AppThemeModule();
export default AppTheme;
