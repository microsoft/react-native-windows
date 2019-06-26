// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import * as React from 'react';
import { View, StyleSheet, Text, TextInput } from 'react-native';
import { supportKeyboard, IKeyboardEvent, CheckBox, ViewWindows, Picker } from '../../src/index.uwp';

// TextInput2 is used to verify supportKeyboard + focus 
const TextInput2 = supportKeyboard(TextInput)

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

interface IKeyboardFocusComponentState {
  selected: string;
  keyOnKeyDown: string;
}

// tslint:disable-next-line
const pickerRef = React.createRef<any>();
const viewWindowsRef = React.createRef<View>();
const textInputRef = React.createRef<TextInput>();
const textInputRef2 = React.createRef<TextInput>();

// tslint:disable-next-line
const checkBoxRef = React.createRef<any>();

class KeyboardFocusExample extends React.Component<{}, IKeyboardFocusComponentState> {
  public constructor(props: {}) {
    super(props);
    this.state = {
      selected: '',
      keyOnKeyDown: 'unknown'
    };
  }

  public render(): JSX.Element {
    const pickerItems = ['View', 'Picker', 'TextInput', 'TextInput2', "CheckBox"];

    return (
      <View style={styles.keyComponentRoot}>
        <Picker selectedValue={this.state.selected} onValueChange={this._selectionChanged}>
          {pickerItems.map(item => (<Picker.Item key={item} value={item} label={item} />))}
        </Picker>

        <ViewWindows style={styles.keyEnterVisualizer} acceptsKeyboardFocus={true} ref={viewWindowsRef}>
          <Text>View accept focus</Text>
        </ViewWindows>

        <Picker ref={pickerRef}>
          <Picker.Item label="Picker accept focus" />
        </Picker>

        <TextInput placeholder='TextInput accept focus' style={styles.textInput} ref={textInputRef} />
        <View>
          <TextInput2 onKeyDown={this._textInputKeyDown} placeholder='TextInput accept focus' style={styles.textInput} ref={textInputRef2} />
          <Text>Key {this.state.keyOnKeyDown}</Text>
        </View>
        <View>
          <CheckBox ref={checkBoxRef} />
          <Text>Checkbox accept focus</Text>
        </View>
      </View>
    );
  }

  private _textInputKeyDown = (ev: IKeyboardEvent) => {
    this.setState({ keyOnKeyDown: ev.nativeEvent.key });
  };

  private _selectionChanged = (selected: string) => {
    switch (selected) {
      case 'View':
        viewWindowsRef.current && viewWindowsRef.current.focus();
        break;
      case 'Picker':
        pickerRef.current && pickerRef.current.focus();
        break;
      case 'TextInput':
        textInputRef.current && textInputRef.current.focus();
        break;
      case 'TextInput2':
        textInputRef2.current && textInputRef2.current.focus();
        break;
      case 'CheckBox':
        checkBoxRef.current && checkBoxRef.current.focus();
        break;
    }
  }
}
export const displayName = (_undefined?: string) => { };
export const title = 'Keyboard Focus Example';
export const description = 'Demo of keyboard focus.';
export const examples = [
  {
    title: 'Keyboard Focus example',
    render(): JSX.Element {
      return <KeyboardFocusExample />;
    },
  }
];