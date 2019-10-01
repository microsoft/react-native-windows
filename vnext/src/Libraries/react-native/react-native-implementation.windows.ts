/*
   This file is used as the root export of this package.

   Since react-native is removing haste as a module lookup mechanism,
   we need to re-export the whole of react-native from react-native-windows.

   We also export the things that are actually exposed to users as part of react-native-windows.
   Following how react-native exports everything, we export using inline requires
   to support the inline require optimizations that the various react-native bundlers use.

   NOTE: when adding exports here, they should also be added to the `typings-index.ts` file,
         which will provide proper type information for these exports.

    @format
*/
import * as ReactNativeWindows from './typings-index';

const ReactNativeImplementation = require('./react-native-implementation.js');

Object.defineProperty(ReactNativeImplementation, 'DatePicker', {
  get: () => {
    return require('../Components/DatePicker/DatePicker')
      .DatePicker as ReactNativeWindows.DatePicker;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Flyout', {
  get: () => {
    return require('../Components/Flyout/Flyout')
      .Flyout as ReactNativeWindows.Flyout;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Glyph', {
  get: () => {
    return require('../Components/Glyph/Glyph')
      .Glyph as ReactNativeWindows.Glyph;
  },
});

Object.defineProperty(ReactNativeImplementation, 'PickerWindows', {
  get: () => {
    return require('../Components/Picker/PickerWindows')
      .Picker as ReactNativeWindows.Picker;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Popup', {
  get: () => {
    return require('../Components/Popup/Popup')
      .Popup as ReactNativeWindows.Popup;
  },
});

Object.defineProperty(ReactNativeImplementation, 'supportKeyboard', {
  get: () => {
    return require('../Components/Keyboard/KeyboardExt').supportKeyboard;
  },
});

Object.defineProperty(ReactNativeImplementation, 'EventPhase', {
  get: () => {
    return require('../Components/Keyboard/KeyboardExtProps')
      .HandledEventPhase as ReactNativeWindows.EventPhase;
  },
});

Object.defineProperty(ReactNativeImplementation, 'HandledEventPhase', {
  get: () => {
    return require('../Components/Keyboard/KeyboardExtProps')
      .HandledEventPhase as ReactNativeWindows.HandledEventPhase;
  },
});

Object.defineProperty(ReactNativeImplementation, 'ViewWindows', {
  get: () => {
    return require('../Components/View/ViewWindows')
      .ViewWindows as ReactNativeWindows.ViewWindows;
  },
});

Object.defineProperty(ReactNativeImplementation, 'AppTheme', {
  get: () => {
    return require('../AppTheme/AppTheme')
      .AppTheme as ReactNativeWindows.AppTheme;
  },
});

export = ReactNativeImplementation;
