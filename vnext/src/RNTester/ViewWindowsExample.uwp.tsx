// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import * as React from 'react';
import { StyleSheet, Text, TextInput } from 'react-native';
import { ViewWindows, IHandledKeyboardEvent, IKeyboardEvent } from '../../src/index.uwp';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black'
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around'
  },
  keyEnterVisualizer: {
    margin: 5,
    alignItems: 'center',
    minWidth: 100,
    minHeight: 30
  },
  textInput: { 
    height: 32, 
    width: 100 
  },
  blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 }
});

interface IKeyboardableComponentState {
  lastKeyDown: string | null;
  lastKeyUp: string | null;
}

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  { key: 'ArrowDown' },
  { key: 'ArrowUp' },
  { key: 'ArrowLeft' },
  { key: 'ArrowRight' },
  { key: 'Tab' }
];

class ViewWindowsKeyboardExample extends React.Component<{}, IKeyboardableComponentState> {
  public constructor(props: {}) {
    super(props);
    this.state = {
      lastKeyDown: null,
      lastKeyUp: null
    };
  }

  public render(): JSX.Element {
    return (
      <ViewWindows keyDownEvents={handledNativeKeyboardEvents} keyUpEvents={handledNativeKeyboardEvents}>
        <ViewWindows
          style={styles.keyComponentRoot}
          onKeyUp={this._onKeyUp}
          onKeyDown={this._onKeyDown}
        >
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyDown</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '}</Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <Text>OnKeyUp</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '}</Text>
          </ViewWindows>
          <ViewWindows style={styles.keyEnterVisualizer}>
            <TextInput placeholder='I got focus' style={styles.textInput}/>
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
}
export const displayName = (_undefined?: string) => {};
export const title = 'ViewWindows Example';
export const description = 'Demo of keyboard properties.';
export const examples = [
  {
    title: 'ViewWindows example',
    render(): JSX.Element {
      return <ViewWindowsKeyboardExample />;
    },
  }
];