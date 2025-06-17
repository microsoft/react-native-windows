/*
 * This file contains the windows extra types, which are not part of core.
 *
 * NOTE: Concrete classes, objects etc that actually need to be exported from the package,
*        need to also be added to index.windows.js
*/

export {Flyout, IFlyoutProps, Placement, ShowMode} from './Components/Flyout/Flyout';
export {Glyph, GlyphProps, GlyphStyle} from './Components/Glyph/Glyph';
export {IPopupProps, Popup} from './Components/Popup/Popup';
export * from './Components/Keyboard/KeyboardExt';
export * from './Components/Keyboard/KeyboardExtProps';
import type {ViewProps} from './Components/View/ViewPropTypes';
export type IViewWindowsProps = ViewProps;
import {View} from './Components/View/View';
export const ViewWindows = View;
import type {View} from './Components/View/View';
export type ViewWindows = View;
export * from './AppTheme/AppThemeTypes';
export * from '../src/private/specs/components/Xaml/XamlHostTypes';
