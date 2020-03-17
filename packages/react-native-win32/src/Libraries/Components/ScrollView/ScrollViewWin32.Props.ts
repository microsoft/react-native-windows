'use strict';
import RN = require('react-native');
import { SharedAccessibilityPropsIOSandWin32, OmittedAccessibilityPropsWin32, BasePropsWin32 } from '../View/ViewWin32.Props';

// removes from interface T the members of interface K
type Omit<T, K> = Pick<T, Exclude<keyof T, keyof K>>;

export type ScrollViewWin32OmitTypes = RN.ScrollViewPropsAndroid &
  RN.ScrollViewPropsIOS &
  RN.AccessibilityPropsAndroid &
  Omit<RN.AccessibilityPropsIOS, SharedAccessibilityPropsIOSandWin32> &
  OmittedAccessibilityPropsWin32;

/**
 * Properties for ViewWin32 component
 */
export interface IScrollViewWin32Props extends Omit<RN.ScrollViewProps, ScrollViewWin32OmitTypes>, BasePropsWin32 {
  biDirectional?: boolean
}
