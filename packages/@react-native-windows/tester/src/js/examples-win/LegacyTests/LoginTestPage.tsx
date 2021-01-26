/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import * as React from 'react'
import { useState } from 'react';
import {
  Text,
  TextInput,
  View,
  StyleSheet,
  TouchableOpacity,
  Button,
} from 'react-native';

const styles = StyleSheet.create({
  container: {
    padding: 20,
  },
  input: {
    height: 40,
    backgroundColor: 'rgba(225,225,225,0.2)',
    marginBottom: 10,
    padding: 10,
    color: '#000000',
  },
  buttonContainer: {
    backgroundColor: '#2980b6',
    paddingVertical: 15,
  },
  buttonText: {
    color: '#fff',
    textAlign: 'center',
    fontWeight: '700',
  },
});

export function LoginTestPage() {
  const [loginState, setLoginState] = useState('');
  const [userName, setUserName] = useState('');
  const [password, setPassword] = useState('');
  const [passwordHidden, setPasswordHidden] = useState(true);

  const onPressShowPassword = () => {
    const previousState = passwordHidden;
    setPasswordHidden(!previousState);
  };

  const onPress = () => {
    if (userName === 'username' && password === 'password') {
      setLoginState('Success');
    } else {
      setLoginState('Fail');
    }
  };
  return (
    <View>
      <TextInput
        style={styles.input}
        placeholder="Email or Mobile Num"
        placeholderTextColor="rgba(225,225,225,0.7)"
        testID="username-field"
        onChange={text => {
          setUserName(text.nativeEvent.text);
        }}
      />

      <TextInput
        style={styles.input}
        placeholder="Password"
        testID="password-field"
        placeholderTextColor="rgba(225,225,225,0.7)"
        secureTextEntry={passwordHidden}
        onChange={text => {
          setPassword(text.nativeEvent.text);
        }}
      />

      <Button
        title={passwordHidden ? 'Show Password' : 'Hide Password'}
        onPress={onPressShowPassword}
        testID='show-password-toggle'
      />

      <TouchableOpacity
        style={styles.buttonContainer}
        testID="submit-button"
        onPress={onPress}
      >
        <Text style={styles.buttonText}>LOGIN</Text>
      </TouchableOpacity>

      <Text testID="result-text">{loginState}</Text>
    </View>
  );
}


export const displayName = (_undefined?: string) => {};
export const title = 'LegacyLoginTest';
export const description = 'Legacy e2e test for TextInput with password';
export const examples = [
  {
    render: function(): JSX.Element {
      return <LoginTestPage />;
    },
  },
];