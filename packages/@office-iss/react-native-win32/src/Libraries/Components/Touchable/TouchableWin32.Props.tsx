import {ViewStyle} from 'react-native';
import {Insets} from 'react-native/types/public/Insets';

import { IPressEvent, IRenderChild, IRenderStyle } from './TouchableWin32.Types';
import { IViewWin32Props, IKeyboardEvent } from '../View/ViewPropTypes.win32';

export interface ITouchableWin32State {
  isPressed: boolean;
  isFocused: boolean;
  isHovered: boolean;
}

type Omit<T, K extends keyof T> = Pick<T, Exclude<keyof T, K>>

export interface ITouchableWin32Props extends Omit <IViewWin32Props, 'children'> {
  rejectResponderTermination?: boolean;
  disabled?: boolean;
  touchableHandleActivePressIn?: (e: IPressEvent) => void;
  touchableHandleActivePressOut?: (e: IPressEvent) => void;
  touchableHandlePress?: (e: IPressEvent) => void;
  touchableHandleKeyPress?: (e: IKeyboardEvent) => void;
  touchableHandleKeyPressDown?: (e: IKeyboardEvent) => void;
  touchableHandleLongPress?: (e: IPressEvent) => void;
  touchableGetHighlightDelayMS?: () => number;
  touchableGetPressRectOffset?: () => Insets;
  touchableGetLongPressDelayMS?: () => number;
  touchableGetPressOutDelayMS?: () => number;
  touchableGetHitSlop?: () => Insets;
  touchSoundDisabled?: boolean;
  onLongPress?: () => void;
  children?: IRenderChild<ITouchableWin32State>;
  // Typescript will not allow an extension of the IView* interface
  // that allows style to take on a function value. This is not a problem
  // with children, presumably because function components are valid as children.
  // As such, a renderStyle prop that takes a function value is provided
  // instead, in conjunction with the base style prop (StyleProp<ViewStyle>).
  // The style prop will only be used if a renderStyle is not provided.
  renderStyle?: IRenderStyle<ITouchableWin32State, ViewStyle>;
  /**
   * Filters keys for touchableHandleKeyPress / touchableHandleKeyPressDown events.
   * Space and Enter keys result in touchableHandleKeyPress(Down) in the event
   * that no filterKeys function is provided as a callback. All keyboard events
   * will result in onKeyUp and onKeyDown getting fired regardless of filterKeys.
   */
  filterKeys?: (str: string) => boolean;
}
