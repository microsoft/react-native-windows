/**
 * @providesModule DatePickerAndroid
 * @flow
 */
'use strict';

// var warning = require('fbjs/lib/warning');

export = {
  async open(options: Object): Promise<Object> {
    return Promise.reject({
      message: 'DatePickerAndroid is not supported on this platform.'
    });
  }
};
