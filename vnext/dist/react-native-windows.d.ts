/**
 * @packagedocumentation
 *
 * This package provides Windows specific components and provides JS implementations for some react-native primitives
 * Cross platform React-native primitives should be imported from 'react-native'
 * Windows specific components need to be imported from 'react-native-windows'
 *
 */

import * as React from 'react';
import * as RN from 'react-native';
import { StyleProp } from 'react-native';
import { View } from 'react-native';
import { ViewProps } from 'react-native';
import { ViewStyle } from 'react-native';

export declare const AppTheme: AppThemeModule;

export declare type AppTheme = AppThemeModule;

declare class AppThemeModule extends MissingNativeEventEmitterShim {
    isAvailable: boolean;
    currentTheme: AppThemeTypes;
    isHighContrast: boolean;
    currentHighContrastColors: IHighContrastColors;
}

export declare type AppThemeTypes = 'light' | 'dark';

export declare class DatePicker extends React.Component<IDatePickerProps> {
    render(): JSX.Element | null;
}

export declare enum DayOfWeek {
    Sunday = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6
}

export declare enum EventPhase {
    None = 0,
    Capturing = 1,
    AtTarget = 2,
    Bubbling = 3
}

export declare class Flyout extends React.Component<IFlyoutProps> {
    render(): JSX.Element | null;
}

export declare class Glyph extends React.PureComponent<GlyphProps> {
    render(): JSX.Element | null;
}

declare interface GlyphProps extends ViewProps {
    style?: StyleProp<GlyphStyle>;
    emSize?: number;
    fontUri: string;
    glyph: string;
    colorEnabled?: boolean;
}

declare interface GlyphStyle extends ViewStyle {
    color?: string;
}

export declare enum HandledEventPhase {
    Capturing = 1,
    Bubbling = 3
}

export declare interface IAppThemeChangedEvent {
    currentTheme: AppThemeTypes;
}

export declare interface IDatePickerChangeEvent {
    nativeEvent: {
        newDate: number;
    };
}

export declare interface IDatePickerProps extends ViewProps {
    selectedDate?: Date;
    placeholderText?: string;
    dateFormat?: 'day month year' | 'dayofweek day month' | 'longdate' | 'shortdate';
    dayOfWeekFormat?: '{dayofweek.abbreviated(2)}' | '{dayofweek.abbreviated(3)}' | '{dayofweek.full}';
    firstDayOfWeek?: DayOfWeek;
    maxDate?: Date;
    minDate?: Date;
    timeZoneOffsetInSeconds?: number;
    onChange?: (event: IDatePickerChangeEvent) => void;
    onDateChange?: (newDate: Date) => void;
}

export declare interface IFlyoutProps extends ViewProps {
    horizontalOffset?: number;
    isLightDismissEnabled?: boolean;
    isOpen?: boolean;
    onDismiss?: (isOpen: boolean) => void;
    placement?: Placement;
    target?: React.ReactNode;
    verticalOffset?: number;
}

export declare interface IHandledKeyboardEvent {
    altKey?: boolean;
    ctrlKey?: boolean;
    metaKey?: boolean;
    shiftKey?: boolean;
    code: string;
    handledEventPhase?: HandledEventPhase;
}

export declare interface IHighContrastChangedEvent {
    isHighContrast: boolean;
    highContrastColors: IHighContrastColors;
}

/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
/**
 * Color information for high contrast
 */
export declare interface IHighContrastColors {
    ButtonFaceColor: string;
    ButtonTextColor: string;
    GrayTextColor: string;
    HighlightColor: string;
    HighlightTextColor: string;
    HotlightColor: string;
    WindowColor: string;
    WindowTextColor: string;
}

export declare type IKeyboardEvent = RN.NativeSyntheticEvent<INativeKeyboardEvent>;

export declare interface IKeyboardProps {
    onKeyDown?: (args: IKeyboardEvent) => void;
    onKeyDownCapture?: (args: IKeyboardEvent) => void;
    onKeyUp?: (args: IKeyboardEvent) => void;
    onKeyUpCapture?: (args: IKeyboardEvent) => void;
    keyDownEvents?: IHandledKeyboardEvent[];
    keyUpEvents?: IHandledKeyboardEvent[];
}

export declare interface INativeKeyboardEvent {
    altKey: boolean;
    ctrlKey: boolean;
    metaKey: boolean;
    shiftKey: boolean;
    key: string;
    code: string;
    eventPhase: EventPhase;
}

declare interface IPickerChangeEvent {
    nativeEvent: {
        value: any;
        itemIndex: number;
        text: string;
    };
}

declare interface IPickerItemProps extends ViewProps {
    label: string;
    value?: any;
    color?: string;
    testID?: string;
}

declare interface IPickerProps extends ViewProps {
    selectedValue?: any;
    enabled?: boolean;
    prompt?: string;
    testID?: string;
    onChange?: (event: IPickerChangeEvent) => void;
    onValueChange?: (value: any, itemIndex: number, text: string) => void;
    editable?: boolean;
    text?: string;
}

export declare interface IPopupProps extends ViewProps {
    isOpen?: boolean;
    isLightDismissEnabled?: boolean;
    horizontalOffset?: number;
    verticalOffset?: number;
    target?: React.ReactNode;
    onDismiss?: (isOpen: boolean) => void;
}

/**
 * @remarks
 * Props type for ViewWindows component
 *
 * Extends: {@link IKeyboardProps} and {@link https://facebook.github.io/react-native/docs/view | react-native's ViewProps}
 */
export declare interface IViewWindowsProps extends IKeyboardProps, ViewProps {
    acceptsKeyboardFocus?: boolean;
    children?: any;
    /**
     * Indicates to accessibility services that the UI Component is within a set and has the given numbered position.
     *
     * See https://github.com/ReactWindows/discussions-and-proposals/blob/harinik-accessibility/proposals/0000-accessibilityapis-lists.md
     */
    accessibilityPosInSet?: number;
    /**
     * Indicates to accessibility services that the UI Component is within a set with the given size.
     *
     * See https://github.com/ReactWindows/discussions-and-proposals/blob/harinik-accessibility/proposals/0000-accessibilityapis-lists.md
     */
    accessibilitySetSize?: number;
}

declare const MissingNativeEventEmitterShim: any;

export declare class Picker extends React.Component<IPickerProps> {
    static Item: typeof PickerItem;
    render(): JSX.Element | null;
}

declare class PickerItem extends React.Component<IPickerItemProps> {
    render(): JSX.Element | null;
}

export declare type Placement = 'top' | 'bottom' | 'left' | 'right' | 'full' | 'top-edge-aligned-left' | 'top-edge-aligned-right' | 'bottom-edge-aligned-left' | 'bottom-edge-aligned-right' | 'left-edge-aligned-top' | 'right-edge-aligned-top' | 'left-edge-aligned-bottom' | 'right-edge-aligned-bottom';

export declare class Popup extends React.Component<IPopupProps> {
    render(): JSX.Element | null;
}

export declare const supportKeyboard: <P extends object>(_Component: React.ComponentType<P>) => {
    new (props: Readonly<P & IKeyboardProps>): {
        render(): JSX.Element | null;
        context: any;
        setState<K extends never>(state: {} | ((prevState: Readonly<{}>, props: Readonly<P & IKeyboardProps>) => {} | Pick<{}, K> | null) | Pick<{}, K> | null, callback?: (() => void) | undefined): void;
        forceUpdate(callBack?: (() => void) | undefined): void;
        readonly props: Readonly<P & IKeyboardProps> & Readonly<{
            children?: React.ReactNode;
        }>;
        state: Readonly<{}>;
        refs: {
            [key: string]: React.ReactInstance;
        };
    };
    new (props: P & IKeyboardProps, context?: any): {
        render(): JSX.Element | null;
        context: any;
        setState<K extends never>(state: {} | ((prevState: Readonly<{}>, props: Readonly<P & IKeyboardProps>) => {} | Pick<{}, K> | null) | Pick<{}, K> | null, callback?: (() => void) | undefined): void;
        forceUpdate(callBack?: (() => void) | undefined): void;
        readonly props: Readonly<P & IKeyboardProps> & Readonly<{
            children?: React.ReactNode;
        }>;
        state: Readonly<{}>;
        refs: {
            [key: string]: React.ReactInstance;
        };
    };
    contextType?: React.Context<any> | undefined;
};

/**
 * Same as {@link https://facebook.github.io/react-native/docs/view | react-native's View}, but with extra windows specific functionality
 *
 * @remarks
 * Prop type: {@link IViewWindowsProps}.
 *
 */
export declare const ViewWindows: ViewWindowsType;

export declare type ViewWindows = ViewWindowsType;

declare type ViewWindowsType = React.ForwardRefExoticComponent<IViewWindowsProps & React.RefAttributes<View>> & View;

export { }
