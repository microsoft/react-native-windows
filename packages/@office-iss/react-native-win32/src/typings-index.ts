/**
 * @packagedocumentation
 *
 * This package provides Win32 specific components in addition to providing the core react-native primities.
 * Cross platform React-native primitives should be imported from 'react-native'
 * Win32 specific components need to be imported from '\@office-iss/react-native-win32'
 *
 */

/*
   This file is used to provide the typings for this package.
   NOTE: Concrete classes, objects etc that actually need to be exported from the package,
         need to also be added to index.win32.js
*/

// Importing from a copy of react-native types instead of from react-native
// to allow custom typescript resolvers to redirect react-native to react-native-win32
// when building bundles for the win32 platform
export * from './rntypes/index';
export type { 
      AccessibilityActionInfo,
      AccessibilityActionName,
      AccessibilityState
} from './Libraries/Components/View/ViewWin32.Props';
export * from './Libraries/Components/View/ViewWin32.Props';
export * from './Libraries/Components/View/ViewWin32';
export * from './Libraries/Components/Text/TextWin32.Props';
export * from './Libraries/Components/Text/TextWin32';
export * from './Libraries/Components/Button/ButtonWin32.Props';
export * from './Libraries/Components/Button/ButtonWin32';
export * from './Libraries/StyleSheet/PlatformColorValueTypesWin32';
export * from './Libraries/Components/Touchable/TouchableWin32.Props';
export * from './Libraries/Components/Touchable/TouchableWin32.Types';
export * from './Libraries/Components/Touchable/TouchableWin32';
export * from './Libraries/PersonaCoin/PersonaCoin';
export * from './Libraries/PersonaCoin/PersonaCoinTypes';
export * from './Libraries/Utilities/FocusManager.win32';