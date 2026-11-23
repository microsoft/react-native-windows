/*
 * This file contains the windows extra types, which are not part of core.
 *
 * NOTE: Concrete classes, objects etc that actually need to be exported from the package,
*        need to also be added to index.windows.js
*/

export {
  HandledEventPhase,
  type IHandledKeyboardEvent,
  type IKeyboardEvent,
} from './Components/View/ViewPropTypes';
import type {ViewProps} from './Components/View/ViewPropTypes';
export type IViewWindowsProps = ViewProps;
import {View} from './Components/View/View';
export const ViewWindows = View;
import type {View} from './Components/View/View';
export type ViewWindows = View;
export * from './AppTheme/AppThemeTypes';
export {InternalTextInputWithCustomType} from './Components/TextInput/TextInput';

export const TextInputViewSpec: Readonly<{
  uiViewClassName: string;
  bubblingEventTypes: Readonly<
    Record<
      string,
      Readonly<{
        phasedRegistrationNames: Readonly<{
          bubbled: string;
          captured: string;
        }>;
      }>
    >
  >;
  directEventTypes: Readonly<
    Record<string, Readonly<{registrationName: string}>>
  >;
  validAttributes: Readonly<
    Record<
      string,
      | true
      | Readonly<{
          process: typeof import('./StyleSheet/processColor').processColor;
        }>
    >
  >;
}>;