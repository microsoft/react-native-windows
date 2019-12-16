import RN = require('react-native');
import { SharedAccessibilityPropsIOSandWin32, OmittedAccessibilityPropsWin32, BasePropsWin32 } from '../View/ViewWin32.Props';

// removes from interface T the members of interface K
type Omit<T, K> = Pick<T, Exclude<keyof T, keyof K>>;

export type TextWin32OmitTypes = RN.TextPropsAndroid &
  RN.TextPropsIOS &
  RN.AccessibilityPropsAndroid &
  Omit<RN.AccessibilityPropsIOS, SharedAccessibilityPropsIOSandWin32> &
  OmittedAccessibilityPropsWin32;

/**
 * Role-based text style names.
 */
export type TextStyle =
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

export interface ITextWin32Props extends Omit<RN.TextProps, TextWin32OmitTypes>, BasePropsWin32 {
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
  textStyle?: TextStyle;

  /** Tooltip displayed on mouse hover of this element */
  tooltip?: string;
}
