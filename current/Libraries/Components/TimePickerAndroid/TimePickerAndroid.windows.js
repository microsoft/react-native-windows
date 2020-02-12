/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule TimePickerAndroid
 * @flow
 */
'use strict';

const TimePickerAndroid = {
  async open(options: Object): Promise<Object> {
    return Promise.reject({
      message: 'TimePickerAndroid is not supported on this platform.'
    });
  },
};

module.exports = TimePickerAndroid;
