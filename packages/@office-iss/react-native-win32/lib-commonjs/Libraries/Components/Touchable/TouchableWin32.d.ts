import React from 'react';
import { ITouchableWin32Props, ITouchableWin32State } from './TouchableWin32.Props';
interface IInternalTouchableWin32State extends ITouchableWin32State {
    isKeyPressed: boolean;
}
/**
 * TouchableWin32 is a 'componentization' of the Touchable Mixin in React Native.
 * This means that instead of implementing components such as TouchableHighlight
 * via the mixin, they are merely implemented as wrappers around TouchableWin32,
 * forwarding the correct set of props. Additionally, TouchableWin32 supports hover
 * via onMouseEnter and onMouseLeave and focus/blur via onFocus/onBlur.
 * TouchableWin32 also allows for functions as child components (that use the internal
 * state of the touchable to conditionally render children) as well functions as styles
 * (that use internal state to conditionally calculate styles)
 */
export declare class TouchableWin32 extends React.Component<ITouchableWin32Props, IInternalTouchableWin32State> {
    private _longPressDelayTimeout;
    private _touchableDelayTimeout;
    private _pressOutDelayTimeout;
    private _pressInLocation;
    private _touchState;
    private _responderID;
    private _positionOnActivate;
    private _dimensionsOnActivate;
    private readonly _internalRef;
    constructor(props: any);
    componentWillUnmount(): void;
    render(): JSX.Element;
    focus: () => void;
    /**
     * The rejectResponderTermination prop provides a way to accept/reject termination requests
     */
    private readonly _touchableHandleResponderTerminationRequest;
    /**
     * Only reject an opportunity to become the responder on bubble if disabled
     */
    private readonly _touchableHandleStartShouldSetResponder;
    /** TODO: Long press cancel as a prop may be a good idea */
    private readonly _touchableLongPressCancelsPress;
    /**
     * On responder being granted, state and local data need to be set
     */
    private readonly _touchableHandleResponderGrant;
    /**
     * Handle responder release
     */
    private readonly _touchableHandleResponderRelease;
    /**
     * Handle responder terminate
     */
    private readonly _touchableHandleResponderTerminate;
    /**
     * Handles move events
     */
    private readonly _touchableHandleResponderMove;
    /**
     * Used while performing side effects during state transitions,
     * to maintain proper bounding dimensions and positional information
     */
    private readonly _remeasureMetricsOnActivation;
    /**
     * Callback into measure, see _remeasureMetricsOnActivation
     */
    private readonly _handleQueryLayout;
    private readonly _handleDelay;
    private readonly _handleLongDelay;
    /**
     * Manages state transitions
     */
    private readonly _receiveSignal;
    private readonly _cancelLongPressDelayTimeout;
    private readonly _isHighlight;
    private readonly _savePressInLocation;
    private readonly _getDistanceBetweenPoints;
    /**
     * Any highlighting/visual effects is done here,
     * This is also where press callbacks are invoked from
     */
    private readonly _performSideEffectsForTransition;
    private readonly _startHighlight;
    private readonly _endHighlight;
    private readonly _onMouseEnter;
    private readonly _onMouseLeave;
    private readonly _onFocus;
    private readonly _onBlur;
    private readonly _onKeyDown;
    private readonly _onKeyUp;
    private readonly _deriveStateFromInternalState;
    private readonly _filterOnKey;
}
export {};
