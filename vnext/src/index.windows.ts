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

const ReactNativeImplementation = require('./index.js');

Object.defineProperty(ReactNativeImplementation, 'DatePicker', {
  get: () => {
    return require('./Libraries/Components/DatePicker/DatePicker')
      .DatePicker as ReactNativeWindows.DatePicker;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Flyout', {
  get: () => {
    return require('./Libraries/Components/Flyout/Flyout')
      .Flyout as ReactNativeWindows.Flyout;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Glyph', {
  get: () => {
    return require('./Libraries/Components/Glyph/Glyph')
      .Glyph as ReactNativeWindows.Glyph;
  },
});

Object.defineProperty(ReactNativeImplementation, 'PickerWindows', {
  get: () => {
    return require('./Libraries/Components/Picker/PickerWindows')
      .Picker as ReactNativeWindows.Picker;
  },
});

Object.defineProperty(ReactNativeImplementation, 'Popup', {
  get: () => {
    return require('./Libraries/Components/Popup/Popup')
      .Popup as ReactNativeWindows.Popup;
  },
});

Object.defineProperty(ReactNativeImplementation, 'supportKeyboard', {
  get: () => {
    return require('./Libraries/Components/Keyboard/KeyboardExt')
      .supportKeyboard;
  },
});

Object.defineProperty(ReactNativeImplementation, 'EventPhase', {
  get: () => {
    return require('./Libraries/Components/Keyboard/KeyboardExtProps')
      .HandledEventPhase as ReactNativeWindows.EventPhase;
  },
});

Object.defineProperty(ReactNativeImplementation, 'HandledEventPhase', {
  get: () => {
    return require('./Libraries/Components/Keyboard/KeyboardExtProps')
      .HandledEventPhase as ReactNativeWindows.HandledEventPhase;
  },
});

Object.defineProperty(ReactNativeImplementation, 'ViewWindows', {
  get: () => {
    return require('./Libraries/Components/View/ViewWindows')
      .ViewWindows as ReactNativeWindows.ViewWindows;
  },
});

Object.defineProperty(ReactNativeImplementation, 'AppTheme', {
  get: () => {
    return require('./Libraries/AppTheme/AppTheme')
      .AppTheme as ReactNativeWindows.AppTheme;
  },
});

export = ReactNativeImplementation;
