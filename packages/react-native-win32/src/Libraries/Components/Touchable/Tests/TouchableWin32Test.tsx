'use strict';

import * as React from 'react';
import { Insets, NativeSyntheticEvent, StyleSheet, Text, ViewStyle } from 'react-native';

import { TextWin32 } from '../../Text/TextWin32';
import { ViewWin32 } from '../../View/ViewWin32';
import { IViewWin32Props, IKeyboardEvent } from '../../View/ViewWin32.Props';
import { TouchableWin32 } from '../TouchableWin32';
import { IPressEvent, IRenderChild } from '../TouchableWin32.Types';
import { ITouchableWin32State } from '../TouchableWin32.Props';

/**
 * Styles used across both examples
 */
const styles = StyleSheet.create({
  largeContainer: {
    height: 200,
    width: 600,
    justifyContent: 'space-between',
    alignItems: 'center',
    flexDirection: 'row',
  },
  smallContainer: {
    height: 90,
    width: 90,
    justifyContent: 'center',
    alignContent: 'center',
  },
  highlight: {
    height: 150,
    width: 150,
    justifyContent: 'space-around',
    alignContent: 'center',
    alignItems: 'center',
    flexDirection: 'row',
  },
  outerTouch: {
    height: 150,
    width: 150,
    justifyContent: 'space-around',
    alignContent: 'center',
    alignItems: 'center',
    flexDirection: 'row',
  },
  innerTouch: {
    height: 90,
    width: 90,
    alignItems: 'center',
    justifyContent: 'center',
  },
});

/**
 * Constant press rect offset, used to demonstrate press geometry
 */
const PRESS_RETENTION_OFFSET: Insets = {
  top: 100,
  left: 100,
  right: 100,
  bottom: 100,
};

interface ITouchableWin32WithoutFeedbackProps extends IViewWin32Props {
  delayLongPress?: number;
  delayPressIn?: number;
  delayPressOut?: number;
  onPress?: (e: IPressEvent) => void;
  onPressIn?: (e: IPressEvent) => void;
  onPressOut?: (e: IPressEvent) => void;
  onLongPress?: (e: IPressEvent) => void;
  pressRetentionOffset?: Insets;
  rejectResponderTermination?: boolean;
}

interface ITouchableWin32WithoutFeedbackState {
  isFocused: boolean;
}

/**
 * Example implementation of TouchableWithoutFeedback - not meant for use outside these examples
 * This is mainly there to show the ability to respond to touches without providing visual feedback,
 * though we do show a red border on focus to know where starts start and end and whether press rect
 * offsets are respected.
 */
class TouchableWin32WithoutFeedback extends React.Component<ITouchableWin32WithoutFeedbackProps, ITouchableWin32WithoutFeedbackState> {
  constructor(props) {
    super(props);

    this.state = { isFocused: false };
  }

  public render() {
    return (
      <TouchableWin32
        focusable
        rejectResponderTermination={true}
        disabled={false}
        touchableHandleActivePressIn={this._touchableHandleActivePressIn}
        touchableHandleActivePressOut={this._touchableHandleActivePressOut}
        touchableHandlePress={this._touchableHandlePress}
        touchableHandleLongPress={this._touchableHandleLongPress}
        touchableGetHighlightDelayMS={this._touchableGetHighlightDelayMS}
        touchableGetPressRectOffset={this._touchableGetPressRectOffset}
        touchableGetLongPressDelayMS={this._touchableGetLongPressDelayMS}
        touchableGetPressOutDelayMS={this._touchableGetPressOutDelayMS}
        touchableGetHitSlop={this._touchableGetHitSlop}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        renderStyle={this._generateStyle}
      >
        {React.Children.only(this.props.children)}
      </TouchableWin32>
    );
  }

  private readonly _touchableHandlePress = (e: IPressEvent) => {
    this.props.onPress && this.props.onPress(e);
  };

  private readonly _touchableHandleActivePressIn = (e: IPressEvent) => {
    this.props.onPressIn && this.props.onPressIn(e);
  };

  private readonly _touchableHandleActivePressOut = (e: IPressEvent) => {
    this.props.onPressOut && this.props.onPressOut(e);
  };

  private readonly _touchableHandleLongPress = (e: IPressEvent) => {
    this.props.onLongPress && this.props.onLongPress(e);
  };

  private readonly _touchableGetPressRectOffset = (): Insets => {
    return this.props.pressRetentionOffset || PRESS_RETENTION_OFFSET;
  };

  private readonly _touchableGetHitSlop = (): Insets => {
    return this.props.hitSlop;
  };

  private readonly _touchableGetHighlightDelayMS = (): number => {
    return this.props.delayPressIn || 0;
  };

  private readonly _touchableGetLongPressDelayMS = (): number => {
    return this.props.delayLongPress === 0 ? 0 : this.props.delayLongPress || 500;
  };

  private readonly _touchableGetPressOutDelayMS = (): number => {
    return this.props.delayPressOut || 0;
  };

  private readonly _onFocus = () => {
    this.setState({ isFocused: true });
  };

  private readonly _onBlur = () => {
    this.setState({ isFocused: false });
  };

  /**
   * The generated style uses hard-coded border width values
   */
  private readonly _generateStyle = (state: ITouchableWin32State): ViewStyle => {
    const finalStyle: ViewStyle = {
      borderWidth: state.isFocused ? 5 : 0,
      borderColor: 'red',
    };
    return Object.assign({}, this.props.style, finalStyle);
  };
}

interface ITouchableWin32HighlightProps extends IViewWin32Props {
  delayLongPress?: number;
  delayLongPressIn?: number;
  delayPressOut?: number;
  onPress?: (e: IPressEvent) => void;
  onPressIn?: (e: IPressEvent) => void;
  onPressOut?: (e: IPressEvent) => void;
  onLongPress?: (e: IPressEvent) => void;
  onKeyPress?: (e: IKeyboardEvent) => void;
  onKeyPressDown?: (e: IKeyboardEvent) => void;
  pressRetentionOffset?: Insets;
  rejectResponderTermination?: boolean;
  underlayColor?: string;
  children?: IRenderChild<ITouchableWin32State>;
}

interface ITouchableWin32HighlightComponentProps extends ITouchableWin32HighlightProps {
  // Used as an imperative handle to the TouchableWin32 interface - primarily for focus()
  innerRef?: React.Ref<TouchableWin32>;
}

/**
 * Example implementation of TouchableHighlight - not meant for use outside these examples
 * The main difference between a ful TouchableHiglight implementation and this example is that
 * TouchableHighlight should manipulate the opacity of the wrapped view to display the underlay color.
 * This example merely uses hard coded color values to distinguish between different control states
 */
class TouchableWin32HighlightComponent extends React.Component<ITouchableWin32HighlightComponentProps, {}> {

  public render() {
    return (
      <TouchableWin32
        focusable
        rejectResponderTermination={this.props.rejectResponderTermination}
        disabled={false}
        touchableHandleActivePressIn={this._touchableHandleActivePressIn}
        touchableHandleActivePressOut={this._touchableHandleActivePressOut}
        touchableHandlePress={this._touchableHandlePress}
        touchableHandleLongPress={this._touchableHandleLongPress}
        touchableGetHighlightDelayMS={this._touchableGetHighlightDelayMS}
        touchableGetPressRectOffset={this._touchableGetPressRectOffset}
        touchableGetLongPressDelayMS={this._touchableGetLongPressDelayMS}
        touchableGetPressOutDelayMS={this._touchableGetPressOutDelayMS}
        touchableGetHitSlop={this._touchableGetHitSlop}
        touchableHandleKeyPress={this._touchableHandleKeyPress}
        touchableHandleKeyPressDown={this._touchableHandleKeyPressDown}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        onMouseEnter={this._mouseEnter}
        onMouseLeave={this._mouseLeave}
        renderStyle={this._generateStyle}
        children={this.props.children}
        ref={this.props.innerRef}
      />
    );
  }

  private readonly _touchableHandlePress = (e: IPressEvent) => {
    this.props.onPress && this.props.onPress(e);
    this.setState({ isPressed: false });
  };
  private readonly _touchableHandleActivePressIn = (e: IPressEvent) => {
    this.props.onPressIn && this.props.onPressIn(e);
    this.setState({ isPressed: true });
  };
  private readonly _touchableHandleActivePressOut = (e: IPressEvent) => {
    this.props.onPressOut && this.props.onPressOut(e);
    this.setState({ isPressed: false });
  };
  private readonly _touchableHandleLongPress = (e: IPressEvent) => {
    this.props.onLongPress && this.props.onLongPress(e);
  };
  private readonly _touchableGetPressRectOffset = (): Insets => {
    return this.props.pressRetentionOffset || PRESS_RETENTION_OFFSET;
  };
  private readonly _touchableGetHitSlop = (): Insets => {
    return { left: 100, right: 100, top: 100, bottom: 100 };
  };
  private readonly _touchableGetHighlightDelayMS = (): number => {
    return this.props.delayLongPressIn || 0;
  };
  private readonly _touchableGetLongPressDelayMS = (): number => {
    return this.props.delayLongPress || 500;
  };
  private readonly _touchableGetPressOutDelayMS = (): number => {
    return this.props.delayPressOut || 0;
  };
  private readonly _touchableHandleKeyPress = (ev: IKeyboardEvent) => {
    this.props.onKeyPress && this.props.onKeyPress(ev);
  }
  private readonly _touchableHandleKeyPressDown = (ev: IKeyboardEvent) => {
    this.props.onKeyPressDown && this.props.onKeyPressDown(ev);
  }
  private readonly _mouseEnter = (): void => {
    this.props.onMouseEnter && this.props.onMouseEnter();
    this.setState({ isMouseIn: true });
  };
  private readonly _mouseLeave = (): void => {
    this.props.onMouseLeave && this.props.onMouseLeave();
    this.setState({ isMouseIn: false });
  };
  private readonly _onFocus = (ev: NativeSyntheticEvent<{}>): void => {
    this.props.onFocus && this.props.onFocus(ev);
    this.setState({ isFocused: true });
  };
  private readonly _onBlur = (ev: NativeSyntheticEvent<{}>): void => {
    this.props.onBlur && this.props.onBlur(ev);
    this.setState({ isFocused: false });
  };

  /**
   * The generated style uses hard-coded color values,
   * though it is trivial to pass these values as props.
   * This is an example of a render style (one that is a
   * funtion passed on to TouchableWin32 and resolved according
   * to the internal state of TouchableWin32).
   */
  private readonly _generateStyle = (state: ITouchableWin32State): ViewStyle => {
    const finalStyle: ViewStyle = {};
    finalStyle.borderColor = state.isFocused ? 'red' : 'blue';
    finalStyle.borderWidth = state.isFocused ? 10 : 5;
    if (state.isHovered) {
      if (state.isPressed) {
        finalStyle.backgroundColor = 'black';
      } else {
        finalStyle.backgroundColor = 'gray';
      }
    } else {
      if (state.isPressed) {
        finalStyle.backgroundColor = 'green';
      } else {
        finalStyle.backgroundColor = 'white';
      }
    }
    return Object.assign({}, this.props.style, finalStyle);
  };
}

// Demonstrating ref forwarding - forwarding a ref using an innerRef prop on a class component
const TouchableWin32Highlight = React.forwardRef<TouchableWin32, ITouchableWin32HighlightProps>(
  (props, ref) => {
    return (
      <TouchableWin32HighlightComponent innerRef={ref} {...props} />
    );
  }
);

/**
 * Both examples merely track number of presses
 */
interface IExampleState {
  numberOfPresses: number;
}

/**
 * TouchableHighlightExamples
 */
class TouchableWithoutFeedbackExample extends React.Component<{}, IExampleState> {
  constructor(props) {
    super(props);
    this.state = { numberOfPresses: 0 };
  }

  public render() {
    return (
      <ViewWin32 style={styles.largeContainer}>
        <TouchableWin32WithoutFeedback style={styles.outerTouch} onPress={this._onPress}>
          <ViewWin32>
            <TextWin32>Press me</TextWin32>
          </ViewWin32>
        </TouchableWin32WithoutFeedback>

        <TouchableWin32WithoutFeedback style={styles.outerTouch} onPress={this._onPress}>
          <ViewWin32 style={styles.smallContainer}>
            <TouchableWin32WithoutFeedback style={styles.innerTouch} onPress={this._onPress}>
              <ViewWin32>
                <TextWin32>You can also have nested Touchables</TextWin32>
              </ViewWin32>
            </TouchableWin32WithoutFeedback>
          </ViewWin32>
        </TouchableWin32WithoutFeedback>
        <TextWin32>{'Number of Recognized Presses: ' + this.state.numberOfPresses}</TextWin32>
      </ViewWin32>
    );
  }

  private readonly _onPress = () => {
    this.setState({ numberOfPresses: this.state.numberOfPresses + 1 });
  };
}

/**
 * TouchableHighlightExamples
 */
class TouchableHighlightExample extends React.Component<{}, IExampleState> {
  constructor(props) {
    super(props);
    this.state = { numberOfPresses: 0 };
  }

  public render() {
    return (
      <ViewWin32 style={styles.largeContainer}>
        <TouchableWin32Highlight style={styles.outerTouch} onPress={this._onPress}>
          <ViewWin32>
            <TextWin32>Press me</TextWin32>
          </ViewWin32>
        </TouchableWin32Highlight>

        <TouchableWin32Highlight style={styles.outerTouch} onPress={this._onPress}>
          <ViewWin32 style={styles.smallContainer}>
            <TouchableWin32Highlight style={styles.innerTouch} onPress={this._onPress} children={this._getChildrenOfInnerTouchable} />
          </ViewWin32>
        </TouchableWin32Highlight>
        <TextWin32 style={{ color: 'red' }} textStyle="None">
          {'Number of Recognized Presses: ' + this.state.numberOfPresses}
        </TextWin32>
      </ViewWin32>
    );
  }

  /**
   * This is primarily to demonstrate render children as a function
   * of state, here we change text color depending on interaction state.
   */
  private readonly _getChildrenOfInnerTouchable = (state: ITouchableWin32State) => {
    return (
      <ViewWin32
        style={{
          borderColor: state.isFocused ? 'green' : 'pink',
          borderWidth: state.isHovered ? 10 : 5,
          height: 70,
          width: 70,
        }}
      >
        <TextWin32
          style={{
            color: this._pickColor(state.isHovered, state.isPressed),
            fontSize: state.isFocused ? 8 : 10,
          }}
          textStyle="None"
        >
          Press Me!
        </TextWin32>
      </ViewWin32>
    );
  };

  private _pickColor(hovered: boolean, pressed: boolean) {
    if (hovered) {
      if (pressed) {
        return 'white';
      } else {
        return 'red';
      }
    } else {
      if (pressed) {
        return 'purple';
      } else {
        return 'black';
      }
    }
  }

  private readonly _onPress = () => {
    this.setState({ numberOfPresses: this.state.numberOfPresses + 1 });
  };
}

const TouchableFocusExample = () => {
  const [focused, setFocused] = React.useState(false);
  const focusableRef = React.useRef<TouchableWin32>(null);

  // onPress callback
  const focusOnPress = React.useCallback(() => {
    focusableRef.current && focusableRef.current.focus();
    focused || setFocused(true);
  }, [focused]);

  // onFocus and onBlur callbacks
  const onFocus = React.useCallback(() => {
    setFocused(true);
  }, []);
  const onBlur = React.useCallback(() => {
    setFocused(false);
  }, []);

  return (
    <ViewWin32 style={styles.largeContainer}>
      <TouchableWin32Highlight onPress={focusOnPress}>
        <Text>Press me to focus my friend</Text>
      </TouchableWin32Highlight>

      <TouchableWin32Highlight
        ref={focusableRef}
        onFocus={onFocus}
        onBlur={onBlur}
      >
        <Text>{'Focused: ' + focused}</Text>
      </TouchableWin32Highlight>
    </ViewWin32>
  );
}

export const displayName = 'TouchableWin32 Examples';
export const title = 'TouchableWin32';
export const description = 'Demonstration of touchable + focus + hover behavior all in one component';

export const examples = [
  {
    title: 'TouchableWithoutFeedback Example',
    description: 'A simple example implementation of without feedback behavior',
    render(): JSX.Element {
      return <TouchableWithoutFeedbackExample />;
    },
  },
  {
    title: 'TouchableHighlight Example',
    description: 'A simple example implementation of highlight behavior',
    render(): JSX.Element {
      return <TouchableHighlightExample />;
    },
  },
  {
    title: 'Imperative Focus on TouchableWin32 Example',
    description: 'A simple example implementation of imperative focus behavior',
    render(): JSX.Element {
      return <TouchableFocusExample />;
    },
  }
];
