/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import Platform from '../Utilities/Platform';

declare var console: typeof console & {_isPolyfilled: boolean, ...};

/**
 * Sets up developer tools for React Native.
 * You can use this module directly, or just require InitializeCore.
 */
if (__DEV__) {
  // TODO (T45803484) Enable devtools for bridgeless RN
  if (!global.RN$Bridgeless) {
    if (!global.__RCTProfileIsProfiling) {
      require('./setUpReactDevTools');

      // Set up inspector
      const JSInspector = require('../JSInspector/JSInspector');
      JSInspector.registerAgent(require('../JSInspector/NetworkAgent'));
    }

    if (!Platform.isTesting) {
      const HMRClient = require('../Utilities/HMRClient');

      if (console._isPolyfilled) {
        // We assume full control over the console and send JavaScript logs to Metro.
        [
          'trace',
          'info',
          'warn',
          'log',
          'group',
          'groupCollapsed',
          'groupEnd',
          'debug',
        ].forEach(level => {
          const originalFunction = console[level];
          console[level] = function(...args) {
            HMRClient.log(level, args);
            originalFunction.apply(console, args);
          };
        });
      } else {
        // We assume the environment has a real rich console (like Chrome), and don't hijack it to log to Metro.
        // It's likely the developer is using rich console to debug anyway, and hijacking it would
        // lose the filenames in console.log calls: https://github.com/facebook/react-native/issues/26788.
        HMRClient.log('log', [
          `JavaScript logs will appear in your ${
            isLikelyARealBrowser ? 'browser' : 'environment'
          } console`,
        ]);
      }
    }

    // [Windows
    // require('./setUpReactRefresh');
    // Windows]
  }
}
