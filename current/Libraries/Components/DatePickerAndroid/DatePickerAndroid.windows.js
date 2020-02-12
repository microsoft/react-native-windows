/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule DatePickerAndroid
 * @flow
 */
'use strict';

const DatePickerAndroid = {
  async open(options: Object): Promise<Object> {
    return Promise.reject({
      message: 'DatePickerAndroid is not supported on this platform.'
    });
  },
};

module.exports = DatePickerAndroid;
