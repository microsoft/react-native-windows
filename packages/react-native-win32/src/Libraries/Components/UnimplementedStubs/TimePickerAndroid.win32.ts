/**
 * @providesModule TimePickerAndroid
 * @flow
 */
'use strict';

// var warning = require('fbjs/lib/warning');

export = {
  async open(options: Object): Promise<Object> {
    return Promise.reject({
      message: 'TimePickerAndroid is not supported on this platform.'
    });
  }
};
