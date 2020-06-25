/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 *
 * This file is part of the root export of this package.
 *
 * It is used to export additional win32 functionality that has been implemented in flow files (thus allowing type checking with core react-native files)
 *
 * NOTE: when adding exports here, they should also be added to the `typings-index.ts` file,
 *       which will provide proper type information for these exports.
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

Object.defineProperty(Index, 'ColorGradientWin32', {
  get: () => {
    return require('./Libraries/StyleSheet/PlatformColorValueTypesWin32')
      .ColorGradientWin32;
  },
});

Object.defineProperty(Index, 'TouchableWin32', {
  get: () => {
    return require('./Libraries/Components/Touchable/TouchableWin32')
      .TouchableWin32;
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
    return require('./Libraries/PersonaCoin/PersonaCoinTypes')
      .PersonaCoinPresence;
  },
});

module.exports = Index;
