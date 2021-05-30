import { Insets, ViewStyle } from 'react-native';
import { IPressEvent, IRenderChild, IRenderStyle } from './TouchableWin32.Types';
import { IViewWin32Props, IKeyboardEvent } from '../View/ViewWin32.Props';
export interface ITouchableWin32State {
    isPressed: boolean;
    isFocused: boolean;
    isHovered: boolean;
}
export interface ITouchableWin32Props extends IViewWin32Props {
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
    renderStyle?: IRenderStyle<ITouchableWin32State, ViewStyle>;
    /**
     * Filters keys for touchableHandleKeyPress / touchableHandleKeyPressDown events.
     * Space and Enter keys result in touchableHandleKeyPress(Down) in the event
     * that no filterKeys function is provided as a callback. All keyboard events
     * will result in onKeyUp and onKeyDown getting fired regardless of filterKeys.
     */
    filterKeys?: (str: string) => boolean;
}
