/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {View, StyleSheet, Text, TextInput} from 'react-native';
import {
  HandledEventPhase,
  IHandledKeyboardEvent,
  IKeyboardEvent,
  supportKeyboard,
} from 'react-native-windows';

const ViewWindows = supportKeyboard(View);

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black',
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around',
  },
  keyEnterVisualizer: {
    margin: 5,
    alignItems: 'center',
    minWidth: 100,
    minHeight: 30,
  },
  textInput: {
    height: 32,
    width: 100,
  },
  blackbox: {height: 30, width: 30, borderColor: 'black', borderWidth: 3},
});

interface IKeyboardableComponentState {
  lastKeyDown: string | null;
  lastKeyUp: string | null;
  lastKeyDownCapture: string | null;
  lastKeyUpCapture: string | null;
  lastKeyDownCode: string | null;
  lastKeyUpCode: string | null;
  lastKeyDownCaptureCode: string | null;
  lastKeyUpCaptureCode: string | null;
}

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  {code: 'KeyA', handledEventPhase: HandledEventPhase.Capturing},
  {code: 'KeyB'},
  {code: 'Digit1', handledEventPhase: HandledEventPhase.Bubbling},
  {code: 'Tab', handledEventPhase: HandledEventPhase.Capturing},
];

class ViewWindowsKeyboardExample extends React.Component<
  {},
  IKeyboardableComponentState
> {
  public constructor(props: {}) {
    super(props);
    this.state = {
      lastKeyDown: null,
      lastKeyUp: null,
      lastKeyDownCapture: null,
      lastKeyUpCapture: null,
      lastKeyDownCode: null,
      lastKeyUpCode: null,
      lastKeyDownCaptureCode: null,
      lastKeyUpCaptureCode: null,
    };
  }

  public render(): JSX.Element {
    return (
      <ViewWindows>
        <ViewWindows
          style={styles.keyComponentRoot}
          onKeyDownCapture={this._onKeyDownCapture}
          onKeyUpCapture={this._onKeyUpCapture}
          onKeyUp={this._onKeyUp}
          onKeyDown={this._onKeyDown}
          keyDownEvents={handledNativeKeyboardEvents}
          keyUpEvents={handledNativeKeyboardEvents}>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyDown Key and Code</Text>
            <Text>
              {this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '}
            </Text>
            <Text>
              {this.state.lastKeyDownCode !== null
                ? this.state.lastKeyDownCode
                : ' '}
            </Text>
            <Text>OnKeyDownCapture Key and Code</Text>
            <Text>
              {this.state.lastKeyDownCapture !== null
                ? this.state.lastKeyDownCapture
                : ' '}
            </Text>
            <Text>
              {this.state.lastKeyDownCaptureCode !== null
                ? this.state.lastKeyDownCaptureCode
                : ' '}
            </Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyUp Key and Code</Text>
            <Text>
              {this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '}
            </Text>
            <Text>
              {this.state.lastKeyUpCode !== null
                ? this.state.lastKeyUpCode
                : ' '}
            </Text>

            <Text>OnKeyUpCapture Key and Code</Text>
            <Text>
              {this.state.lastKeyUpCapture !== null
                ? this.state.lastKeyUpCapture
                : ' '}
            </Text>
            <Text>
              {this.state.lastKeyUpCaptureCode !== null
                ? this.state.lastKeyUpCaptureCode
                : ' '}
            </Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <TextInput placeholder="I got focus" style={styles.textInput} />
          </ViewWindows>
        </ViewWindows>
      </ViewWindows>
    );
  }

  private readonly _onKeyUp = (ev: IKeyboardEvent) => {
    this.setState({
      lastKeyUp: ev.nativeEvent.key,
      lastKeyDown: null,
      lastKeyUpCode: ev.nativeEvent.code,
      lastKeyDownCode: null,
    });
  };

  private readonly _onKeyDown = (ev: IKeyboardEvent) => {
    this.setState({
      lastKeyDown: ev.nativeEvent.key,
      lastKeyUp: null,
      lastKeyDownCode: ev.nativeEvent.code,
      lastKeyUpCode: null,
    });
  };
  private readonly _onKeyUpCapture = (ev: IKeyboardEvent) => {
    this.setState({
      lastKeyUpCapture: ev.nativeEvent.key,
      lastKeyDownCapture: null,
      lastKeyUpCaptureCode: ev.nativeEvent.code,
      lastKeyDownCaptureCode: null,
    });
  };

  private readonly _onKeyDownCapture = (ev: IKeyboardEvent) => {
    this.setState({
      lastKeyDownCapture: ev.nativeEvent.key,
      lastKeyUpCapture: null,
      lastKeyDownCaptureCode: ev.nativeEvent.code,
      lastKeyUpCaptureCode: null,
    });
  };
}
export const displayName = (_undefined?: string) => {};
export const title = 'Keyboard extension Example';
export const category = 'Basic';
export const description = 'Demo of keyboard properties.';
export const examples = [
  {
    title: 'Keyboard extension example',
    render(): JSX.Element {
      return <ViewWindowsKeyboardExample />;
    },
  },
];
