/**
 * This is primarily a fork of React Native's Touchable Mixin.
 * It has been repurposed as it's own standalone control for win32,
 * as it needs to support a richer set of functionality on the desktop.
 * The touchable variants can be rewritten as wrappers around TouchableWin32
 * by passing the correct set of props down and managing state correctly.
 *
 * React Native's Touchable.js file (https://github.com/facebook/react-native/blob/master/Libraries/Components/Touchable/Touchable.js)
 * provides an overview over how touchables work and interact with the gesture responder system.
 */
'use strict';

import * as React from 'react';
import { NativeSyntheticEvent, StyleProp, UIManager, ViewStyle } from 'react-native';

import { ITouchableWin32Props, ITouchableWin32State } from './TouchableWin32.Props';
import {
  IChildAsFunction,
  IDimensions,
  IPosition,
  IPressEvent,
  IPressInLocation,
  ISignal,
  IState,
  IStateConditions,
  ITransitions,
} from './TouchableWin32.Types';
import { ViewWin32 } from '../View/ViewWin32';
import { IKeyboardEvent } from '../View/ViewWin32.Props';

const BoundingDimensions = require('./BoundingDimensions');
const Position = require('./Position');

const {findNodeHandle} = require('../../Renderer/shims/ReactNative');

/**
 * Extracts a single touch, generally this is the active touch or touch that
 * has just ended
 * @param e - a press event containing information about the native event
 */
function extractSingleTouch(e: IPressEvent) {
  const nativeEvent = e.nativeEvent;
  const touches = nativeEvent.touches;
  const changedTouches = nativeEvent.changedTouches;
  const hasTouches = touches && touches.length > 0;
  const hasChangedTouches = changedTouches && changedTouches.length > 0;

  return !hasTouches && hasChangedTouches ? changedTouches[0] : hasTouches ? touches[0] : nativeEvent;
}

/** Quick lookup maps for a variety of conditions */
const baseStatesCondition: IStateConditions = {
  NOT_RESPONDER: false,
  RESPONDER_INACTIVE_PRESS_IN: false,
  RESPONDER_INACTIVE_PRESS_OUT: false,
  RESPONDER_ACTIVE_PRESS_IN: false,
  RESPONDER_ACTIVE_PRESS_OUT: false,
  RESPONDER_ACTIVE_LONG_PRESS_IN: false,
  RESPONDER_ACTIVE_LONG_PRESS_OUT: false,
  ERROR: false,
};

const IsActive: IStateConditions = {
  ...baseStatesCondition,
  RESPONDER_ACTIVE_PRESS_OUT: true,
  RESPONDER_ACTIVE_PRESS_IN: true,
};

const IsPressingIn: IStateConditions = {
  ...baseStatesCondition,
  RESPONDER_INACTIVE_PRESS_IN: true,
  RESPONDER_ACTIVE_PRESS_IN: true,
  RESPONDER_ACTIVE_LONG_PRESS_IN: true,
};

const IsLongPressingIn: IStateConditions = {
  ...baseStatesCondition,
  RESPONDER_ACTIVE_LONG_PRESS_IN: true,
};

const transitions: ITransitions = {
  NOT_RESPONDER: {
    DELAY: 'ERROR',
    RESPONDER_GRANT: 'RESPONDER_INACTIVE_PRESS_IN',
    RESPONDER_RELEASE: 'ERROR',
    RESPONDER_TERMINATED: 'ERROR',
    ENTER_PRESS_RECT: 'ERROR',
    LEAVE_PRESS_RECT: 'ERROR',
    LONG_PRESS_DETECTED: 'ERROR',
  },
  RESPONDER_INACTIVE_PRESS_IN: {
    DELAY: 'RESPONDER_ACTIVE_PRESS_IN',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_INACTIVE_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_INACTIVE_PRESS_OUT',
    LONG_PRESS_DETECTED: 'ERROR',
  },
  RESPONDER_INACTIVE_PRESS_OUT: {
    DELAY: 'RESPONDER_ACTIVE_PRESS_OUT',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_INACTIVE_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_INACTIVE_PRESS_OUT',
    LONG_PRESS_DETECTED: 'ERROR',
  },
  RESPONDER_ACTIVE_PRESS_IN: {
    DELAY: 'ERROR',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_ACTIVE_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_ACTIVE_PRESS_OUT',
    LONG_PRESS_DETECTED: 'RESPONDER_ACTIVE_LONG_PRESS_IN',
  },
  RESPONDER_ACTIVE_PRESS_OUT: {
    DELAY: 'ERROR',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_ACTIVE_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_ACTIVE_PRESS_OUT',
    LONG_PRESS_DETECTED: 'ERROR',
  },
  RESPONDER_ACTIVE_LONG_PRESS_IN: {
    DELAY: 'ERROR',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_ACTIVE_LONG_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_ACTIVE_LONG_PRESS_OUT',
    LONG_PRESS_DETECTED: 'RESPONDER_ACTIVE_LONG_PRESS_IN',
  },
  RESPONDER_ACTIVE_LONG_PRESS_OUT: {
    DELAY: 'ERROR',
    RESPONDER_GRANT: 'ERROR',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'RESPONDER_ACTIVE_LONG_PRESS_IN',
    LEAVE_PRESS_RECT: 'RESPONDER_ACTIVE_LONG_PRESS_OUT',
    LONG_PRESS_DETECTED: 'ERROR',
  },
  ERROR: {
    DELAY: 'NOT_RESPONDER',
    RESPONDER_GRANT: 'RESPONDER_INACTIVE_PRESS_IN',
    RESPONDER_RELEASE: 'NOT_RESPONDER',
    RESPONDER_TERMINATED: 'NOT_RESPONDER',
    ENTER_PRESS_RECT: 'NOT_RESPONDER',
    LEAVE_PRESS_RECT: 'NOT_RESPONDER',
    LONG_PRESS_DETECTED: 'NOT_RESPONDER',
  },
};

// Touchable maintains additional internal state to manage keypress interactions
// This information is abstracted away when passed to children off the touchable
// (function children) or styles. A keypressed state is equivalent to a normal
// press state. If either a pointer, mouse, or special key (space/enter) is pressed
// the touchable considers itself to be in a pressed state. Do note that key presses
// will not invoke gesture responder hooksand are not subject to the passed in delays.
interface IInternalTouchableWin32State extends ITouchableWin32State {
  isKeyPressed: boolean;
}

/** Constants for integrating into UI Components */
const HIGHLIGHT_DELAY_MS = 130;
const PRESS_EXPAND_DIPS = 20; // Subject to change for win32 and desktop
const LONG_PRESS_THRESHOLD = 500;
const LONG_PRESS_DELAY_MS = LONG_PRESS_THRESHOLD - HIGHLIGHT_DELAY_MS;
const LONG_PRESS_ALLOWED_MOVEMENT = 10;

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
export class TouchableWin32 extends React.Component<ITouchableWin32Props, IInternalTouchableWin32State> {
  private _longPressDelayTimeout: number;
  private _touchableDelayTimeout: number;
  private _pressOutDelayTimeout: number;
  private _pressInLocation: IPressInLocation;
  private _touchState: IState;
  private _responderID: number;
  private _positionOnActivate: IPosition;
  private _dimensionsOnActivate: IDimensions;

  private readonly _internalRef: React.RefObject<ViewWin32>

  constructor(props) {
    super(props);

    this.state = {
      isKeyPressed: false,
      isPressed: false,
      isFocused: false,
      isHovered: false,
    };

    this._internalRef = React.createRef<ViewWin32>();
  }

  public componentWillUnmount() {
    this._touchableDelayTimeout && clearTimeout(this._touchableDelayTimeout);
    this._longPressDelayTimeout && clearTimeout(this._longPressDelayTimeout);
    this._pressOutDelayTimeout && clearTimeout(this._pressOutDelayTimeout);
  }

  public render() {
    const { children, style, renderStyle, ...rest } = this.props;

    // The React Native touchables generally clone the child component
    // to apply additional styles to it. This approach allows children
    // to be passed in as functions (to be invoked with the current state),
    // it is handy for manipulating children as a result of the interaction
    // state (e.g. text color). A similar approach is taken for style
    let child: React.ReactNode;
    if (children) {
      child = typeof children === 'function' ?
        (children as IChildAsFunction<ITouchableWin32State>)(this._deriveStateFromInternalState()) : children;

      // Continue to enforce single child constraint
      child = React.Children.only(child);
    }

    let computedStyle: StyleProp<ViewStyle>;
    if (renderStyle) {
      computedStyle = renderStyle(this._deriveStateFromInternalState());
    } else if (style) {
      computedStyle = style;
    }

    return (
      <ViewWin32
        // Forward most of the props
        {...rest}
        // Hooks into Gesture Responder System
        onStartShouldSetResponder={this._touchableHandleStartShouldSetResponder}
        onResponderTerminationRequest={this._touchableHandleResponderTerminationRequest}
        onResponderGrant={this._touchableHandleResponderGrant}
        onResponderMove={this._touchableHandleResponderMove}
        onResponderRelease={this._touchableHandleResponderRelease}
        onResponderTerminate={this._touchableHandleResponderTerminate}
        // Hover
        onMouseEnter={this._onMouseEnter}
        onMouseLeave={this._onMouseLeave}
        // Focus
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        // Key press interactions
        onKeyDown={this._onKeyDown}
        onKeyUp={this._onKeyUp}
        // Ref
        ref={this._internalRef}
        // Style
        style={computedStyle}
      >
        {child}
      </ViewWin32>
    );
  }

  public focus = () => {
    this._internalRef.current.focus();
  }

  /**
   * The rejectResponderTermination prop provides a way to accept/reject termination requests
   */
  private readonly _touchableHandleResponderTerminationRequest = (): boolean => {
    return !this.props.rejectResponderTermination;
  };

  /**
   * Only reject an opportunity to become the responder on bubble if disabled
   */
  private readonly _touchableHandleStartShouldSetResponder = (): boolean => {
    return !this.props.disabled;
  };

  /** TODO: Long press cancel as a prop may be a good idea */
  private readonly _touchableLongPressCancelsPress = (): boolean => {
    return true;
  };

  /**
   * On responder being granted, state and local data need to be set
   */
  private readonly _touchableHandleResponderGrant = (e: IPressEvent): void => {
    const dispatchID = findNodeHandle(e.currentTarget);
    e.persist();

    this._pressOutDelayTimeout && clearTimeout(this._pressOutDelayTimeout);
    this._pressOutDelayTimeout = null;

    this._touchState = 'NOT_RESPONDER';
    this._responderID = dispatchID;

    this._receiveSignal('RESPONDER_GRANT', e);
    let delayMS: number = this.props.touchableGetHighlightDelayMS
      ? Math.max(this.props.touchableGetHighlightDelayMS(), 0)
      : HIGHLIGHT_DELAY_MS;
    delayMS = isNaN(delayMS) ? HIGHLIGHT_DELAY_MS : delayMS;

    if (delayMS !== 0) {
      this._touchableDelayTimeout = setTimeout(this._handleDelay.bind(this, e), delayMS);
    } else {
      this._handleDelay(e);
    }

    let longDelayMS: number = this.props.touchableGetLongPressDelayMS
      ? Math.max(this.props.touchableGetLongPressDelayMS(), 10)
      : LONG_PRESS_DELAY_MS;
    longDelayMS = isNaN(longDelayMS) ? LONG_PRESS_DELAY_MS : longDelayMS;
    this._longPressDelayTimeout = setTimeout(this._handleLongDelay.bind(this, e), longDelayMS + delayMS);
  };

  /**
   * Handle responder release
   */
  private readonly _touchableHandleResponderRelease = (e: IPressEvent) => {
    this._pressInLocation = null;
    this._receiveSignal('RESPONDER_RELEASE', e);
  };

  /**
   * Handle responder terminate
   */
  private readonly _touchableHandleResponderTerminate = (e: IPressEvent) => {
    this._pressInLocation = null;
    this._receiveSignal('RESPONDER_TERMINATED', e);
  };

  /**
   * Handles move events
   */
  private readonly _touchableHandleResponderMove = (e: IPressEvent) => {
    if (!this._positionOnActivate) {
      return;
    }

    const positionOnActivate = this._positionOnActivate;
    const dimensionsOnActivate = this._dimensionsOnActivate;
    const pressRectOffset = this.props.touchableGetPressRectOffset
      ? this.props.touchableGetPressRectOffset()
      : {
        left: PRESS_EXPAND_DIPS,
        right: PRESS_EXPAND_DIPS,
        top: PRESS_EXPAND_DIPS,
        bottom: PRESS_EXPAND_DIPS,
      };

    let pressExpandLeft = pressRectOffset.left;
    let pressExpandTop = pressRectOffset.top;
    let pressExpandRight = pressRectOffset.right;
    let pressExpandBottom = pressRectOffset.bottom;

    // TODO implement touchableGetHitSlop natively
    const hitSlop = this.props.touchableGetHitSlop ? this.props.touchableGetHitSlop() : null;

    if (hitSlop) {
      pressExpandLeft += hitSlop.left || 0;
      pressExpandTop += hitSlop.top || 0;
      pressExpandRight += hitSlop.right || 0;
      pressExpandBottom += hitSlop.bottom || 0;
    }

    const touch = extractSingleTouch(e);
    const pageX = touch && touch.pageX;
    const pageY = touch && touch.pageY;

    if (this._pressInLocation) {
      const movedDistance = this._getDistanceBetweenPoints(pageX, pageY, this._pressInLocation.pageX, this._pressInLocation.pageY);
      if (movedDistance > LONG_PRESS_ALLOWED_MOVEMENT) {
        this._cancelLongPressDelayTimeout();
      }
    }

    const isTouchWithinActive =
      pageX > positionOnActivate.left - pressExpandLeft &&
      pageY > positionOnActivate.top - pressExpandTop &&
      pageX < positionOnActivate.left + dimensionsOnActivate.width + pressExpandRight &&
      pageY < positionOnActivate.top + dimensionsOnActivate.height + pressExpandBottom;
    if (isTouchWithinActive) {
      const prevState = this._touchState;
      this._receiveSignal('ENTER_PRESS_RECT', e);
      const currState = this._touchState;
      if (currState === 'RESPONDER_INACTIVE_PRESS_IN' && prevState !== 'RESPONDER_INACTIVE_PRESS_IN') {
        this._cancelLongPressDelayTimeout();
      }
    } else {
      this._cancelLongPressDelayTimeout();
      this._receiveSignal('LEAVE_PRESS_RECT', e);
    }
  };

  /**
   * Used while performing side effects during state transitions,
   * to maintain proper bounding dimensions and positional information
   */
  private readonly _remeasureMetricsOnActivation = () => {
    const tag = this._responderID;
    if (tag === null) {
      return;
    }

    UIManager.measure(tag, this._handleQueryLayout);
  };

  /**
   * Callback into measure, see _remeasureMetricsOnActivation
   */
  private readonly _handleQueryLayout = (l: number, t: number, w: number, h: number, globalX: number, globalY: number) => {
    if (!l && !t && !w && !h && !globalX && !globalY) {
      return;
    }

    this._positionOnActivate = Position.getPooled(globalX, globalY);
    this._dimensionsOnActivate = BoundingDimensions.getPooled(w, h);
  };

  private readonly _handleDelay = (e: IPressEvent) => {
    this._touchableDelayTimeout = null;
    this._receiveSignal('DELAY', e);
  };

  private readonly _handleLongDelay = (e: IPressEvent) => {
    this._longPressDelayTimeout = null;
    const currState = this._touchState;

    if (currState !== 'RESPONDER_ACTIVE_PRESS_IN' && currState !== 'RESPONDER_ACTIVE_LONG_PRESS_IN') {
      const errorMessage =
        'Attempted to transition from state ' +
        currState +
        ' to ' +
        'RESPONDER_ACTIVE_LONG_PRESS_IN ' +
        'which is not supported. This is most likely due to ' +
        'Touchable.longPressDelayTimeout not being canceled.';
      console.error(errorMessage);
    } else {
      this._receiveSignal('LONG_PRESS_DETECTED', e);
    }
  };

  /**
   * Manages state transitions
   */
  private readonly _receiveSignal = (signal: ISignal, e: IPressEvent) => {
    const responderID = this._responderID;
    const currState = this._touchState;
    const nextState: IState = transitions[currState] ? transitions[currState][signal] : null;

    if (!nextState) {
      const errorMessage = 'Unrecognized signal ' + signal + ' or state ' + currState + ' for Touchable responder ' + responderID + '.';
      throw new Error(errorMessage);
    }
    if (nextState === 'ERROR') {
      const errorMessage = 'Touchable cannot transition from ' + currState + ' to ' + signal + ' for responder ' + responderID + '.';
      throw new Error(errorMessage);
    }

    if (currState !== nextState) {
      this._performSideEffectsForTransition(currState, nextState, signal, e);
      this._touchState = nextState;
    }
  };

  private readonly _cancelLongPressDelayTimeout = () => {
    this._longPressDelayTimeout && clearTimeout(this._longPressDelayTimeout);
    this._longPressDelayTimeout = null;
  };

  private readonly _isHighlight = (state: IState) => {
    return state === 'RESPONDER_ACTIVE_PRESS_IN' || state === 'RESPONDER_ACTIVE_LONG_PRESS_IN';
  };

  private readonly _savePressInLocation = (e: IPressEvent) => {
    const touch = extractSingleTouch(e);
    const pageX = touch && touch.pageX;
    const pageY = touch && touch.pageY;
    const locationX = touch && touch.locationX;
    const locationY = touch && touch.locationY;
    this._pressInLocation = { pageX, pageY, locationX, locationY };
  };

  private readonly _getDistanceBetweenPoints = (aX: number, aY: number, bX: number, bY: number): number => {
    const deltaX = aX - bX;
    const deltaY = aY - bY;
    return Math.sqrt(deltaX * deltaX + deltaY * deltaY);
  };

  /**
   * Any highlighting/visual effects is done here,
   * This is also where press callbacks are invoked from
   */
  // Existing high cyclomatic complexity
  // eslint-disable-next-line complexity
  private readonly _performSideEffectsForTransition = (currState: IState, nextState: IState, signal: ISignal, e: IPressEvent) => {
    const currIsHighlight = this._isHighlight(currState);
    const newIsHighlight = this._isHighlight(nextState);

    const isFinalSignal = signal === 'RESPONDER_TERMINATED' || signal === 'RESPONDER_RELEASE';

    if (isFinalSignal) {
      this._cancelLongPressDelayTimeout();
    }

    const isInitialTransition = currState === 'NOT_RESPONDER' && nextState === 'RESPONDER_INACTIVE_PRESS_IN';

    const isActiveTransition = !IsActive[currState] && IsActive[nextState];
    if (isInitialTransition || isActiveTransition) {
      this._remeasureMetricsOnActivation();
    }

    if (IsPressingIn[currState] && signal === 'LONG_PRESS_DETECTED') {
      this.props.touchableHandleLongPress && this.props.touchableHandleLongPress(e);
    }

    if (newIsHighlight && !currIsHighlight) {
      this._startHighlight(e);
    } else if (!newIsHighlight && currIsHighlight) {
      this._endHighlight(e);
    }

    if (IsPressingIn[currState] && signal === 'RESPONDER_RELEASE') {
      const hasLongPressHandler = !!this.props.onLongPress;
      const pressIsLongButStillCallOnPress = IsLongPressingIn[currState] && (!hasLongPressHandler || this._touchableLongPressCancelsPress);
      const shouldInvokePress = !IsLongPressingIn[currState] || pressIsLongButStillCallOnPress;

      if (shouldInvokePress && this.props.touchableHandlePress) {
        if (!newIsHighlight && !currIsHighlight) {
          // we never highlighted because of delay, but we should highlight now
          this._startHighlight(e);
          this._endHighlight(e);
        }

        this.props.touchableHandlePress(e);
      }
    }

    this._touchableDelayTimeout && clearTimeout(this._touchableDelayTimeout);
    this._touchableDelayTimeout = null;
  };

  private readonly _startHighlight = (e: IPressEvent) => {
    this._savePressInLocation(e);
    this.setState({ isPressed: true });
    this.props.touchableHandleActivePressIn && this.props.touchableHandleActivePressIn(e);
  };

  private readonly _endHighlight = (e: IPressEvent) => {
    function _handler() {
      this.props.touchableHandleActivePressOut(e);
    }

    this.setState({ isPressed: false });
    if (this.props.touchableHandleActivePressOut) {
      if (this.props.touchableGetPressOutDelayMS && this.props.touchableGetPressOutDelayMS()) {
        this._pressOutDelayTimeout = setTimeout(_handler.bind(this), this.props.touchableGetPressOutDelayMS());
      } else {
        this.props.touchableHandleActivePressOut(e);
      }
    }
  };

  private readonly _onMouseEnter = () => {
    this.setState({ isHovered: true });
    this.props.onMouseEnter && this.props.onMouseEnter();
  };

  private readonly _onMouseLeave = () => {
    this.setState({ isHovered: false });
    this.props.onMouseLeave && this.props.onMouseLeave();
  };

  private readonly _onFocus = (ev: NativeSyntheticEvent<{}>) => {
    this.setState({ isFocused: true });
    this.props.onFocus && this.props.onFocus(ev);
  };

  private readonly _onBlur = (ev: NativeSyntheticEvent<{}>) => {
    this.setState({ isFocused: false });
    this.props.onBlur && this.props.onBlur(ev);
  };

  private readonly _onKeyDown = (ev: IKeyboardEvent) => {
    if (this._filterOnKey(ev)) {
      this.setState({ isKeyPressed: true });
      this.props.touchableHandleKeyPressDown && this.props.touchableHandleKeyPressDown(ev);
    }

    this.props.onKeyDown && this.props.onKeyDown(ev);
  };

  private readonly _onKeyUp = (ev: IKeyboardEvent) => {
    if (this._filterOnKey(ev)) {
      this.setState({ isKeyPressed: false });
      this.props.touchableHandleKeyPress && this.props.touchableHandleKeyPress(ev);
    }

    this.props.onKeyUp && this.props.onKeyUp(ev);
  };

  private readonly _deriveStateFromInternalState = (): ITouchableWin32State => {
    return {
      isPressed: this.state.isPressed || this.state.isKeyPressed,
      isHovered: this.state.isHovered,
      isFocused: this.state.isFocused,
    };
  }

  private readonly _filterOnKey = (ev: IKeyboardEvent): boolean => {
    if (this.props.filterKeys) {
      return this.props.filterKeys(ev.nativeEvent.key);
    }

    // Otherwise fall back to handling space and enter only
    return ev.nativeEvent.key === ' ' || ev.nativeEvent.key === 'Enter';
  }
}
