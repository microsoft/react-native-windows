/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import React, { useState } from 'react';
import { Text, TextInput, View, StyleSheet, TouchableOpacity } from 'react-native';
import { USERNAME_ON_LOGIN, PASSWORD_ON_LOGIN, SUBMIT_ON_LOGIN, LOGINRESULT_ON_LOGIN } from './Consts';

const styles = StyleSheet.create({
  container: {
    padding: 20
  },
  input: {
    height: 40,
    backgroundColor: 'rgba(225,225,225,0.2)',
    marginBottom: 10,
    padding: 10,
    color: '#fff'
  },
  buttonContainer: {
    backgroundColor: '#2980b6',
    paddingVertical: 15
  },
  buttonText: {
    color: '#fff',
    textAlign: 'center',
    fontWeight: '700'
  }
});

export function LoginTestPage() {
  const [loginState, setLoginState] = useState('');
  const [userName, setUserName] = useState('');
  const [password, setPassword] = useState('');

  const onPress = () => {
    if (userName === 'username' && password === 'password') {
      setLoginState('Success');
    } else {
      setLoginState('Fail');
    }
  };
  return (
    <View>
      <TextInput style={styles.input}
        placeholder='Email or Mobile Num'
        placeholderTextColor='rgba(225,225,225,0.7)'
        testID={USERNAME_ON_LOGIN}
        onChange={(text) => { setUserName(text.nativeEvent.text) }} />

      <TextInput style={styles.input}
        placeholder='Password'
        testID={PASSWORD_ON_LOGIN}
        placeholderTextColor='rgba(225,225,225,0.7)'
        secureTextEntry 
        onChange={(text) => { setPassword(text.nativeEvent.text) }} />

      <TouchableOpacity style={styles.buttonContainer}
        testID={SUBMIT_ON_LOGIN}
        onPress={onPress}>
        <Text style={styles.buttonText}>LOGIN</Text>
      </TouchableOpacity>

      <Text testID={LOGINRESULT_ON_LOGIN}>{loginState}</Text>
    </View >);
}