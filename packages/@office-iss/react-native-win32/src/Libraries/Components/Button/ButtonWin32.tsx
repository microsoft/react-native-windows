import React from 'react'
import RN = require('react-native');
import type { IViewWin32Props } from '../View/ViewPropTypes.win32';
import type { ITextWin32Props } from '../Text/TextWin32.Props';
import type { IButtonWin32Props } from './ButtonWin32.Props';

const enum SelectState {
  NotSelected,
  Selected
}

interface IButtonWin32State {
  accessibilityState: RN.AccessibilityState;
}

/**
 * React-native <Button> control with additional Win32-specific functionality.
 */
export class ButtonWin32 extends React.Component<IButtonWin32Props, IButtonWin32State> {
  constructor(props: IButtonWin32Props) {
    super(props);
    this.state = this._makeState(SelectState.NotSelected);
  }

  public render() {
    const viewProps: IViewWin32Props = {
      accessible: true,
      accessibilityLabel: this.props.accessibilityLabel || this.props.title,
      accessibilityRole: 'button',
      accessibilityState: this.state.accessibilityState,
      focusable: true,
      onFocus: this._onFocus,
      onBlur: this._onBlur,
      onMouseEnter: this.props.onMouseEnter,
      onMouseLeave: this.props.onMouseLeave,
      // @ts-ignore
      onTouchStart: this.props.onTouchStart,
      onTouchEnd: this._onTouchEnd,
      testID: this.props.testID,
      style: this.props.style as RN.StyleProp<RN.ViewStyle>,
    };

    const textProps: ITextWin32Props = {
      textStyle: 'None',
    };
    if (this.props.color) {
      textProps.style = { color: this.props.color };
    }

    return (
      // @ts-ignore
      <RN.ViewWin32 {...viewProps}>
        <RN.TextWin32 {...textProps}>{this.props.title}</RN.TextWin32>
      </RN.ViewWin32>
    );
  }

  private readonly _makeState = (select: SelectState): IButtonWin32State => {
    return {
      accessibilityState: {
        disabled: this.props.disabled,
        selected: select === SelectState.Selected,
      },
    };
  };

  private readonly _setState = (select: SelectState): void => {
    const state = this._makeState(select);
    this.setState(state);
  };

  private readonly _onFocus = (): void => {
    this._setState(SelectState.Selected);
    if (this.props.onFocus) {
      this.props.onFocus();
    }
  };

  private readonly _onBlur = (): void => {
    this._setState(SelectState.NotSelected);
    if (this.props.onBlur) {
      this.props.onBlur();
    }
  };

  private readonly _onTouchEnd = (event: RN.GestureResponderEvent): void => {
    if (!this.props.disabled) {
      if (this.props.onPress) {
        this.props.onPress(event);
      }
    }
    if (this.props.onTouchEnd) {
      this.props.onTouchEnd(event);
    }
  };
}
