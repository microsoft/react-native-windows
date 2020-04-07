/*
   This file is used as the root export of this package.

   NOTE: when adding exports here, they should also be added to the `typings-index.ts` file,
         which will provide proper type information for these exports.

    @format
*/
import * as ReactNativeWindows from './typings-index';

const ReactNativeImplementation = {};

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
