/*
   This file is used as the root export of this package.

   Since react-native is removing haste as a module lookup mechanism,
   we need to re-export the whole of react-native from react-native-win32.

   We also export the things that are actually exposed to users as part of react-native-win32.
   Following how react-native exports everything, we export using inline requires
   to support the inline require optimizations that the various react-native bundlers use.

   NOTE: when adding exports here, they should also be added to the `typings-index.ts` file,
         which will provide proper type information for these exports.
*/

const ReactNativeImplementation = require('./react-native-implementation.js');

Object.defineProperty(ReactNativeImplementation, 'ViewWin32', {
  get: () => {
    return require('../Components/View/ViewWin32').ViewWin32;
  },
});

Object.defineProperty(ReactNativeImplementation, 'TextWin32', {
  get: () => {
    return require('../Components/Text/TextWin32').TextWin32;
  },
});

Object.defineProperty(ReactNativeImplementation, 'ButtonWin32', {
  get: () => {
    return require('../Components/Button/ButtonWin32').ButtonWin32;
  },
});

Object.defineProperty(ReactNativeImplementation, 'TouchableWin32', {
  get: () => {
    return require('../Components/Touchable/TouchableWin32').TouchableWin32;
  },
});

Object.defineProperty(ReactNativeImplementation, 'PersonaCoin', {
  get: () => {
    return require('../Components/PersonaCoin/PersonaCoin').PersonaCoin;
  },
});

export = ReactNativeImplementation;
