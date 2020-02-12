/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Stub of VibrationIOS for Windows.
 *
 * @providesModule VibrationIOS
 */
'use strict';

var warning = require('fbjs/lib/warning');

var VibrationIOS = {
  vibrate: function() {
    warning('VibrationIOS is not supported on this platform!');
  }
};

module.exports = VibrationIOS;
