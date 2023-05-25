/*
 * This file contains the win32 extra types, which are not part of core.
 *
 * NOTE: Concrete classes, objects etc that actually need to be exported from the package,
*        need to also be added to index.win32.js
*/

import {IViewWin32Props as IViewWin32PropsOnly} from './Components/View/ViewPropTypes.win32';
import { AccessibilityPropsWin32 } from '@office-iss/react-native-win32/Libraries/Components/View/ViewAccessibility.win32';
export { AccessibilityPropsWin32 } from '@office-iss/react-native-win32/Libraries/Components/View/ViewAccessibility.win32';
export type IViewWin32Props = IViewWin32PropsOnly & AccessibilityPropsWin32;
export {ViewWin32} from './Components/View/ViewWin32';
export {IKeyboardEvent, IHandledKeyboardEvent} from './Components/View/ViewPropTypes.win32';
import {ITextWin32Props as ITextWin32PropsOnly} from './Components/Text/TextWin32.Props';
export type ITextWin32Props = ITextWin32PropsOnly & AccessibilityPropsWin32;
export {TextWin32TextStyle } from './Components/Text/TextWin32.Props';
export {TextWin32} from './Components/Text/TextWin32';
export {IButtonWin32Props, IButtonWin32Style} from './Components/Button/ButtonWin32.Props';
export {ButtonWin32} from './Components/Button/ButtonWin32';
export {GradientColorStop, GradientColorValueWin32, ColorGradientWin32} from './StyleSheet/PlatformColorValueTypesWin32';
export {ITouchableWin32Props} from './Components/Touchable/TouchableWin32.Props';
export * from './Components/Touchable/TouchableWin32.Types';
export * from './Components/Touchable/TouchableWin32';
export * from './PersonaCoin/PersonaCoin';
export * from './PersonaCoin/PersonaCoinTypes';
export * from './Utilities/FocusManager.win32';