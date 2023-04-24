/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  AppRegistry,
  Button,
  StyleSheet,
  TextInput,
  Keyboard,
  View,
  KeyboardAvoidingView,
  ScrollView,
} from 'react-native';

import type {EventSubscription} from 'react-native/Libraries/vendor/emitter/EventEmitter';

export default class Bootstrap extends React.Component<{}, any> {
  didShowEmitterSubscription = {} as EventSubscription;
  didHideEmitterSubscription = {} as EventSubscription;

  componentDidMount() {
    this.didShowEmitterSubscription = Keyboard.addListener(
      'keyboardDidShow',
      this.keyboardDidShow,
    );
    this.didHideEmitterSubscription = Keyboard.addListener(
      'keyboardDidHide',
      this.keyboardDidHide,
    );
  }

  keyboardDidShow = () => {
    console.log('keyboardDidShow');
  };

  keyboardDidHide = () => {
    console.log('keyboardDidHide');
  };

  componentWillUnmount() {
    this.didShowEmitterSubscription.remove();
    this.didHideEmitterSubscription.remove();
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
    let textInputRef: TextInput | null;
    return (
      <ScrollView>
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
            ref={ref => (textInputRef = ref)}
            onFocus={() => setTimeout(() => textInputRef?.blur(), 5000)}
            placeholder={'blurs after 5 seconds'}
            style={styles.input}
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
          <TextInput
            placeholder="Single line"
            cursorColor="#00FF00"
            placeholderTextColor="grey"
            style={[
              styles.input,
              {backgroundColor: 'black', color: 'white', marginBottom: 4},
            ]}
          />
          <TextInput
            placeholder="Single line with caret color and caret hidden"
            cursorColor="#00FF00"
            caretHidden={true}
            placeholderTextColor="grey"
            style={[
              styles.input,
              {backgroundColor: 'black', color: 'white', marginBottom: 4},
            ]}
          />
          <TextInput
            multiline={true}
            placeholder="Multiline"
            cursorColor="#00FF00"
            placeholderTextColor="grey"
            style={[
              styles.input,
              {backgroundColor: 'black', color: 'white', marginBottom: 4},
            ]}
          />
          <TextInput
            placeholder="Single line with selection color"
            cursorColor="#00FF00"
            selectionColor="yellow"
            placeholderTextColor="grey"
            style={[
              styles.input,
              {backgroundColor: 'black', color: 'white', marginBottom: 4},
            ]}
          />
          <TextInput
            multiline={true}
            placeholder="Multiline with selection color"
            cursorColor="#00FF00"
            selectionColor="yellow"
            placeholderTextColor="grey"
            style={[styles.input, {backgroundColor: 'black', color: 'white'}]}
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
      </ScrollView>
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
