/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  TextInput,
  Keyboard,
  View,
  KeyboardAvoidingView,
} from 'react-native';

export default class Bootstrap extends React.Component<{}, any> {
  componentDidMount() {
    Keyboard.addListener('keyboardDidShow', this.keyboardDidShow);

    Keyboard.addListener('keyboardDidHide', this.keyboardDidHide);
  }

  keyboardDidShow = () => {
    console.log('keyboardDidShow');
  };

  keyboardDidHide = () => {
    console.log('keyboardDidHide');
  };

  componentWillUnmount() {
    Keyboard.removeListener('keyboardDidShow', this.keyboardDidShow);

    Keyboard.removeListener('keyboardDidHide', this.keyboardDidHide);
  }

  state = {
    passwordHidden: true,
    text: '',
  };

  onPressShowPassword = () => {
    const previousState = this.state.passwordHidden;
    this.setState({passwordHidden: !previousState});
  };

  render() {
    return (
      <View style={styles.container}>
        <TextInput
          style={styles.input}
          placeholder={'MultiLine'}
          multiline={true}
        />
        <TextInput
          style={styles.input}
          placeholder={'ReadOnly'}
          editable={false}
        />
        <TextInput
          style={styles.input}
          placeholder={'SpellChecking Disabled'}
          spellCheck={false}
        />
        <TextInput
          style={styles.input}
          placeholder={'PlaceHolder color blue'}
          placeholderTextColor="blue"
        />
        <TextInput
          style={styles.input}
          placeholder={'contextMenuHidden'}
          contextMenuHidden={true}
        />
        <TextInput
          style={styles.input}
          caretHidden={true}
          placeholder={'caretHidden'}
        />
        <TextInput
          style={styles.input}
          keyboardType="number-pad"
          placeholder={'number-pad keyboardType'}
        />
        <TextInput
          style={styles.input}
          autoCapitalize="characters"
          placeholder={'autoCapitalize characters'}
        />
        <TextInput
          style={styles.input}
          placeholder={this.state.passwordHidden ? 'Password' : 'Text'}
          autoCapitalize="none"
          secureTextEntry={this.state.passwordHidden}
          onChangeText={text => {
            this.setState({text});
          }}
          value={this.state.text}
          selectionColor="red"
          maxLength={10}
          keyboardType="numeric"
        />
        <Button
          title={
            this.state.passwordHidden
              ? 'SecureTextEntry On'
              : 'SecureTextEntry Off'
          }
          onPress={this.onPressShowPassword}
        />
        <KeyboardAvoidingView
          style={styles.container}
          behavior="padding"
          enabled>
          <TextInput
            style={styles.input}
            placeholder={'KeyboardAvoidingView padding'}
          />
        </KeyboardAvoidingView>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    paddingTop: 23,
    flex: 1,
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
  },
  input: {
    margin: 5,
    width: 700,
    borderColor: '#7a42f4',
    borderWidth: 1,
    fontSize: 15,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
