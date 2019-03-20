// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

var warning = require('fbjs/lib/warning');

var VibrationIOS = {
  vibrate: function() {
    warning('VibrationIOS is not supported on this platform!');
  }
};

module.exports = VibrationIOS;
