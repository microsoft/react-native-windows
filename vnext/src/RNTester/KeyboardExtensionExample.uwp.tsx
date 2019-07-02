// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import * as React from 'react';
import { View, StyleSheet, Text, TextInput } from 'react-native';
import { supportKeyboard, IHandledKeyboardEvent, IKeyboardEvent, HandledEventPhase } from '../../src/index.uwp';

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
  blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 },
});

interface IKeyboardableComponentState {
  lastKeyDown: string | null;
  lastKeyUp: string | null;
  lastKeyDownCapture: string | null;
  lastKeyUpCapture: string | null;
}

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  { key: 'a', handledEventPhase: HandledEventPhase.Capturing },
  { key: 'b' },
  { key: 'c', handledEventPhase: HandledEventPhase.Bubbling },
  { key: 'Tab', handledEventPhase: HandledEventPhase.Capturing },
];

class ViewWindowsKeyboardExample extends React.Component<{}, IKeyboardableComponentState> {
  public constructor(props: {}) {
    super(props);
    this.state = {
      lastKeyDown: null,
      lastKeyUp: null,
      lastKeyDownCapture: null,
      lastKeyUpCapture: null,
    };
  }

  public render(): JSX.Element {
    return (
      <ViewWindows >
        <ViewWindows
          style={styles.keyComponentRoot}
          onKeyDownCapture={this._onKeyDownCapture}
          onKeyUpCapture={this._onKeyUpCapture}
          onKeyUp={this._onKeyUp}
          onKeyDown={this._onKeyDown}
          keyDownEvents={handledNativeKeyboardEvents}
          keyUpEvents={handledNativeKeyboardEvents}
        >
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyDown</Text>
            <Text>{this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '}</Text>
            <Text>OnKeyDownCapture</Text>
            <Text>{this.state.lastKeyDownCapture !== null ? this.state.lastKeyDownCapture : ' '}</Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyUp</Text>
            <Text>{this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '}</Text>
            <Text>OnKeyUpCapture</Text>
            <Text>{this.state.lastKeyUpCapture !== null ? this.state.lastKeyUpCapture : ' '}</Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <TextInput placeholder="I got focus" style={styles.textInput} />
          </ViewWindows>
        </ViewWindows>
      </ViewWindows>
    );
  }

  private _onKeyUp = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyUp: ev.nativeEvent.key, lastKeyDown: null });
  };

  private _onKeyDown = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyDown: ev.nativeEvent.key, lastKeyUp: null });
  };
  private _onKeyUpCapture = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyUpCapture: ev.nativeEvent.key, lastKeyDownCapture: null });
  };

  private _onKeyDownCapture = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyDownCapture: ev.nativeEvent.key, lastKeyUpCapture: null });
  };
}
export const displayName = (_undefined?: string) => { };
export const title = 'Keyboard extension Example';
export const description = 'Demo of keyboard properties.';
export const examples = [
  {
    title: 'Keyboard extension example',
    render(): JSX.Element {
      return <ViewWindowsKeyboardExample />;
    },
  },
];
