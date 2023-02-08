import RN = require('react-native');
import type { IKeyboardEvent, IHandledKeyboardEvent } from '../View/ViewPropTypes.win32';

/**
 * Role-based text style names.
 */
export type TextWin32TextStyle =
  | 'None'
  | 'SmallStandard'
  | 'SmallSecondary'
  | 'MediumStandard'
  | 'MediumSecondary'
  | 'MediumApp'
  | 'MediumBold'
  | 'MediumBoldApp'
  | 'LargeStandard'
  | 'LargePlusStandard'
  | 'ExtraLargeStandard'
  | 'HugeStandard';

export interface ITextWin32Props extends RN.TextProps {
  onKeyDown?: (args: IKeyboardEvent) => void;
  onKeyDownCapture?: (args: IKeyboardEvent) => void;
  onKeyUp?: (args: IKeyboardEvent) => void;
  onKeyUpCapture?: (args: IKeyboardEvent) => void;

  keyDownEvents?: IHandledKeyboardEvent[];
  keyUpEvents?: IHandledKeyboardEvent[];

  /** Enables a focusable label with copyability but without character selectability (property:selectable) */
  focusable?: boolean;

  /**
   * The onBlur event occurs when an element loses focus.  The opposite of onBlur is onFocus.  Note that in React
   * Native, unlike in the web, the onBlur event bubbles (similar to onFocusOut in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onBlur?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onBlur event occurs when an element loses focus.  The opposite of onBlur is onFocus.  Note that in React
   * Native, unlike in the web, the onBlur event bubbles (similar to onFocusOut in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onBlurCapture?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onFocus event occurs when an element gets focus. The opposite of onFocus is onBlur.  Note that in React
   * Native, unlike in the web, the onFocus event bubbles (similar to onFocusIn in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onFocus?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onFocus event occurs when an element gets focus. The opposite of onFocus is onBlur.  Note that in React
   * Native, unlike in the web, the onFocus event bubbles (similar to onFocusIn in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onFocusCapture?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  
  /**
   * Role-based styling of the text control. The styles applied include
   * font face, size, weight and color. These styles take precedence over
   * the `style` property.
   *
   * @remarks
   * The default value is `MediumStandard`.
   *
   * When set to `None`, role-based styling is disabled.
   *
   * @deprecated Use `style` instead.
   */
  textStyle?: TextWin32TextStyle;

  /** Tooltip displayed on mouse hover of this element */
  tooltip?: string;
}
