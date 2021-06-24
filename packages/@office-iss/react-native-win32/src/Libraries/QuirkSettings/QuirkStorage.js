/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import NativeQuirkStorage from './NativeQuirkStorage';

let constantsCache: ?{[key: string]: string | boolean | number} = null;

const constants: typeof NativeQuirkStorage.getConstants = () => {
  if (constantsCache) {
    return constantsCache;
  } else if (NativeQuirkStorage) {
    constantsCache = NativeQuirkStorage.getConstants();
    return constantsCache;
  } else {
    constantsCache = {};
    return constantsCache;
  }
};

const QuirkStorage = {
  enableFocusAwareRealization: () => {
    return constants().enableFocusAwareRealization;
  },
};

export default QuirkStorage;
