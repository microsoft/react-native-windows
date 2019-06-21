// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';
const NativeModules = require('NativeModules');
const ThemingNative = NativeModules.ThemingModule;

export class Theming extends NativeEventEmitter  {

  constructor() {
    super(ThemingNative);
  }

  get isHighContrast(): boolean {
    return ThemingNative.isHighContrast;
  }

  get currentTheme(): string {
    return ThemingNative.currentTheme();
  }
}

export default Theming;