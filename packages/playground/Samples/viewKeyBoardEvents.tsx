/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {useCallback, useEffect, useRef, useState} from 'react';
import {
  AppRegistry,
  NativeSyntheticEvent,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import {INativeKeyboardEvent, ViewWindows} from 'react-native-windows';

const styles = StyleSheet.create({
  container: {
    height: 500,
    justifyContent: 'space-evenly',
    paddingLeft: 100,
  },
  button: {
    width: 200,
    height: 50,
    flexDirection: 'row',
    alignContent: 'center',
    borderColor: 'blue',
    borderWidth: 2,
    borderRadius: 8,
  },
});

const Bootstrap = () => {
  const [lastKeyUp, setLastKeyUp] = useState('');
  const [lastKeyDown, setLastKeyDown] = useState('');
  const buttonRef = useRef<ViewWindows>(null);

  const onKeyUp = useCallback(
    (event: NativeSyntheticEvent<INativeKeyboardEvent>) => {
      setLastKeyUp(event.nativeEvent.key);
    },
    [setLastKeyUp],
  );

  const onKeyDown = useCallback(
    (event: NativeSyntheticEvent<INativeKeyboardEvent>) => {
      setLastKeyDown(event.nativeEvent.key);
    },
    [setLastKeyDown],
  );

  useEffect(() => {
    buttonRef.current?.focus();
  }, []);

  return (
    <View style={styles.container}>
      <Text>{`The last keyDown received was ${lastKeyDown}`}</Text>
      <Text>{`The last keyUp received was ${lastKeyUp}`}</Text>

      <ViewWindows
        ref={buttonRef}
        style={styles.button}
        onKeyDown={onKeyDown}
        onKeyUp={onKeyUp}
        focusable={true}>
        <Text>{'focus me and press keys on keyboard.'}</Text>
      </ViewWindows>
    </View>
  );
};

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
