/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule ToastAndroid
 * @noflow
 */
'use strict';

var warning = require('fbjs/lib/warning');

var ToastAndroid = {

  show: function (
    message: string,
    duration: number
  ): void {
    warning(false, 'ToastAndroid is not supported on this platform.');
  },

};

module.exports = ToastAndroid;
