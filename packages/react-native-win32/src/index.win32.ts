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

const Index = require('./index.js');

Object.defineProperty(Index, 'ViewWin32', {
  get: () => {
    return require('./Libraries/Components/View/ViewWin32').ViewWin32;
  },
});

Object.defineProperty(Index, 'TextWin32', {
  get: () => {
    return require('./Libraries/Components/Text/TextWin32').TextWin32;
  },
});

Object.defineProperty(Index, 'ButtonWin32', {
  get: () => {
    return require('./Libraries/Components/Button/ButtonWin32').ButtonWin32;
  },
});

Object.defineProperty(Index, 'TouchableWin32', {
  get: () => {
    return require('./Libraries/Components/Touchable/TouchableWin32').TouchableWin32;
  },
});

Object.defineProperty(Index, 'PersonaCoin', {
  get: () => {
    return require('./Libraries/PersonaCoin/PersonaCoin').PersonaCoin;
  },
});

Object.defineProperty(Index, 'PersonaCoinSize', {
  get: () => {
    return require('./Libraries/PersonaCoin/PersonaCoinTypes').PersonaCoinSize;
  },
});

Object.defineProperty(Index, 'PersonaCoinPresence', {
  get: () => {
    return require('./Libraries/PersonaCoin/PersonaCoinTypes').PersonaCoinPresence;
  },
});

export = Index;
