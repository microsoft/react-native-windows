import RN = require('react-native');

// removes from interface T the members of interface K
type Omit<T, K> = Pick<T, Exclude<keyof T, keyof K>>;

export type ButtonWin32OmitStyles = RN.TextStyleIOS & RN.TextStyleAndroid;

export interface IButtonWin32Style extends Omit<RN.TextStyle, ButtonWin32OmitStyles> {}

export interface IButtonWin32Props extends RN.ButtonProps {
  /**
   * Style information which controls the visual appearence of the button.
   */
  style?: RN.StyleProp<IButtonWin32Style>;

  /**
   * Blur event. Called when focus is lost.
   */
  onBlur?: () => void;

  /**
   * Focus event. Called when focus is acquired.
   */
  onFocus?: () => void;

  /**
   * Mouse-enter event. Called when mouse is moved on to the control.
   */
  onMouseEnter?: () => void;

  /**
   * Mouse-leave event. Called when mouse is moved off of the control.
   */
  onMouseLeave?: () => void;

  /**
   * Touch-start event. Called when the user begins touching the control.
   */
  onTouchStart?: (event: RN.GestureResponderEvent) => void;

  /**
   * Touch-end event. Called when the user stops touching the control.
   */
  onTouchEnd?: (event: RN.GestureResponderEvent) => void;
}
