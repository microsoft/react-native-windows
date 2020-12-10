/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

'use strict';

import * as React from 'react';
import {findNodeHandle, NativeModules} from 'react-native';

/*
 ** This is a helper class intended to allow usage of Polite/Aggressive Focus for win32.
 */
class FocusManager {
  // This function takes in a ref to a React Component and a bool value. If setWindowFocus = true, call aggressive focus.
  // Else, call polite focus

  static focus(ref: React.Ref<any>, setWindowFocus: boolean) {
    if (ref) {
      if (setWindowFocus) {
        NativeModules.UIManager.dispatchViewManagerCommand(
          findNodeHandle(ref),
          NativeModules.UIManager.getViewManagerConfig('RCTView').Commands
            .aggressivefocus,
          null,
        );
      } else {
        NativeModules.UIManager.dispatchViewManagerCommand(
          findNodeHandle(ref),
          NativeModules.UIManager.getViewManagerConfig('RCTView').Commands
            .politefocus,
          null,
        );
      }
    }
  }
}

module.exports = FocusManager;
