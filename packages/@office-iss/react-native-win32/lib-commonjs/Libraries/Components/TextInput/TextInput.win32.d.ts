/**
 * This is a forked and slightly modified version of React Native's TextInput.
 * The fork is necessary as platform checks in the base implementation made the
 * control unusable on win32. In addition to cleaning up some of the code, this
 * fork also uses Typescript rather than Flow for type safety.
 *
 * More general documentation on this control can be found at
 * https://facebook.github.io/react-native/docs/textinput.html
 *
 * The original implementation can be found at
 * https://github.com/facebook/react-native/blob/1013a010492a7bef5ff58073a088ac924a986e9e/Libraries/Components/TextInput/TextInput.js
 *
 * This control does not support the full React Native TextInput interface yet.
 * Most of the work necessary to make that happen needs to happen on the native side.
 * Future work on the JS side may include:
 * 1. Expanded typings for some of the events
 * 2. Additional work to manage selection
 * 3. Any base/default styling work
 */
import React from 'react';
import { TextInputProps } from 'react-native';
declare class TextInput extends React.Component<TextInputProps, {}> {
    private _rafID;
    private readonly _inputRef;
    private _lastNativeText;
    private _eventCount;
    constructor(props: any);
    /**
     * On mount TextInput needs to register itself with TextInputState
     * and conditionally request an animation frame for focus.
     */
    componentDidMount(): void;
    /**
     * This is an unfortunate consequence of having controlled TextInputs.
     * Tree diffing reconciliation will not always send down text values
     * This sets text explicitly.
     */
    componentDidUpdate(): void;
    /**
     * Pre-unmoun the TextInput should blur, unregister and clean up
     * the animation frame for focus (edge cases)
     */
    componentWillUnmount(): void;
    render(): JSX.Element;
    /**
     * Returns true if the TextInput is focused
     */
    isFocused(): boolean;
    /**
     * Focuses the TextInput
     */
    focus: () => void;
    /**
     * Blurs the TextInput
     */
    blur: () => void;
    /**
     * Use clear for programmatic clearing of the text
     */
    clear: () => void;
    private readonly setEventCount;
    private readonly setNativeText;
    private readonly _getText;
    private readonly _onChange;
    private readonly _onFocus;
    private readonly _onBlur;
}
export = TextInput;
