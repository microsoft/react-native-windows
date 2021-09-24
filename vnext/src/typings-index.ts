/**
 * @packagedocumentation
 *
 * This package provides Windows specific components in addition to providing the core react-native primities.
 * Cross platform React-native primitives should be imported from 'react-native'
 * Windows specific components need to be imported from 'react-native-windows'
 *
 */

/*
   This file is used to provide the typings for this package.
   NOTE: Concrete classes, objects etc that actually need to be exported from the package,
         need to also be added to index.windows.js
*/

// Importing from a copy of react-native types instead of from react-native
// to allow custom typescript resolvers to redirect react-native to react-native-windows
// when building bundles for the windows platform
export * from './rntypes/index';
export * from './Libraries/Components/Flyout/FlyoutProps';
export * from './Libraries/Components/Flyout/Flyout';
export * from './Libraries/Components/Glyph/Glyph';
export * from './Libraries/Components/Popup/PopupProps';
export * from './Libraries/Components/Popup/Popup';
export * from './Libraries/Components/Keyboard/KeyboardExt';
export * from './Libraries/Components/Keyboard/KeyboardExtProps';
export * from './Libraries/Components/View/ViewWindowsProps';
export * from './Libraries/Components/View/ViewWindows';
export * from './Libraries/AppTheme/AppTheme';
export * from './Libraries/AppTheme/AppThemeTypes';
