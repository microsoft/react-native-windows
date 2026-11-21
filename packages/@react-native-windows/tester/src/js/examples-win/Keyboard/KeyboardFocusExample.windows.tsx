/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  StyleSheet,
  Text,
  TouchableHighlight,
  TouchableOpacity,
} from 'react-native';
import {
  TextInput,
  KeyDownEvent,
  TextInputInstance,
  View,
  ViewInstance,
} from 'react-native-windows';
import {Picker} from '@react-native-picker/picker';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black',
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'column',
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

const KeyboardFocusExample: React.FunctionComponent = () => {
  const pickerItems = [
    'View',
    'Picker',
    'TextInput',
    'TextInput2',
    'TouchableHighlight',
    'TouchableOpacity',
  ];

  const [selected, setSelected] = React.useState<string>('');
  const [keyOnKeyDown, setKeyOnKeyDown] = React.useState<string>('unknown');
  const [focusMessageHighlight, setFocusMessageHighlight] =
    React.useState<string>('');
  const [focusMessageOpacity, setFocusMessageOpacity] =
    React.useState<string>('');
  const pickerRef = React.useRef<Picker<string>>(null);
  const viewWindowsRef = React.useRef<ViewInstance>(null);
  const textInputRef = React.useRef<TextInputInstance>(null);
  const textInputRef2 = React.useRef<TextInputInstance>(null);
  const touchableHighlightRef = React.useRef<React.ComponentRef<typeof TouchableHighlight>>(null);
  const touchableOpacityRef = React.useRef<React.ComponentRef<typeof TouchableOpacity>>(null);

  const selectionChanged = React.useCallback(
    (selected: string) => {
      setSelected(selected);

      switch (selected) {
        case 'View':
          viewWindowsRef.current?.focus();
          break;
        case 'Picker':
          // pickerRef.current && pickerRef.current.focus();
          break;
        case 'TextInput':
          textInputRef.current?.focus();
          setTimeout(() => {
            textInputRef2.current?.blur();
          }, 10000);
          break;
        case 'TextInput2':
          textInputRef2.current?.focus();
          setTimeout(() => {
            textInputRef2.current?.blur();
          }, 10000);
          break;
        case 'TouchableHighlight':
          touchableHighlightRef.current?.focus();
          break;
        case 'TouchableOpacity':
          touchableOpacityRef.current?.focus();
          break;
      }
    },
    [setSelected],
  );

  return (
    <View style={styles.keyComponentRoot}>
      <View>
        <Text>Please select a item to set focus</Text>
      </View>
      <Picker selectedValue={selected} onValueChange={selectionChanged}>
        {pickerItems.map(item => (
          <Picker.Item key={item} value={item} label={item} />
        ))}
      </Picker>

      <View
        style={styles.keyEnterVisualizer}
        focusable={true}
        ref={viewWindowsRef}>
        <Text>View accept focus</Text>
      </View>

      <View>
        <Picker ref={pickerRef}>
          <Picker.Item label="Picker accept focus" />
        </Picker>
      </View>
      <View>
        <Text>
          Test Purpose: focus on TextInput, then timeout and blur on TextInput2,
          TextInput still keep focus
        </Text>
        <TextInput
          placeholder="TextInput accept focus"
          style={styles.textInput}
          ref={textInputRef}
        />
      </View>
      <View>
        <Text>
          Test Purpose: focus on TextInput2, then timeout and blur on
          TextInput2, TextInput2 lose focus
        </Text>
        <TextInput
          onKeyDown={(e: KeyDownEvent) => setKeyOnKeyDown(e.nativeEvent.key)}
          placeholder="TextInput accept focus"
          style={styles.textInput}
          ref={textInputRef2}
        />
        <Text>Key {keyOnKeyDown}</Text>
      </View>
      <View>
        <TouchableHighlight
          ref={touchableHighlightRef}
          onFocus={() => setFocusMessageHighlight('TouchableHighlight onFocus')}
          onBlur={() => setFocusMessageHighlight('TouchableHighlight onBlur')}>
          <View style={{width: 50, height: 50, backgroundColor: 'blue'}} />
        </TouchableHighlight>
        <Text>
          Last focus event for TouchableHighlight: {focusMessageHighlight}
        </Text>
        <TouchableOpacity
          ref={touchableOpacityRef}
          onFocus={() => setFocusMessageOpacity('TouchableOpacity onFocus')}
          onBlur={() => setFocusMessageOpacity('TouchableOpacity onBlur')}>
          <View style={{width: 50, height: 50, backgroundColor: 'yellow'}} />
        </TouchableOpacity>
        <Text>
          Last focus event for TouchableOpacity: {focusMessageOpacity}
        </Text>
      </View>
    </View>
  );
};

export const displayName = (_undefined?: string) => {};
export const title = 'Keyboard Focus Example';
export const category = 'Basic';
export const description = 'Demo of keyboard focus.';
export const examples = [
  {
    title: 'Keyboard Focus example',
    render() {
      return <KeyboardFocusExample />;
    },
  },
];
