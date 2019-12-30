/**
 * @providesModule Settings
 * @flow
 */
'use strict';

export = {
  // tslint:disable-next-line no-reserved-keywords -- get name matching facebook implementation
  get(key: string) {
    console.warn('Settings is not yet supported on win32');
    return null;
  },

  // tslint:disable-next-line no-reserved-keywords -- set name matching facebook implementation
  set(settings: Object) {
    console.warn('Settings is not yet supported on win32');
  },

  watchKeys(keys: string | Array<string>, callback: Function): number {
    console.warn('Settings is not yet supported on win32');
    return -1;
  },

  clearWatch(watchId: number) {
    console.warn('Settings is not yet supported on win32');
  }
};
