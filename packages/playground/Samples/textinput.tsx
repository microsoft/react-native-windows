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
  TouchableWithoutFeedback,
  Alert,
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
    endEditingText: '',
  };

  onPressShowPassword = () => {
    const previousState = this.state.passwordHidden;
    this.setState({passwordHidden: !previousState});
  };

  handleEndEditing = (event: any) => {
    const text = event.nativeEvent.text;
    this.setState({endEditingText: text});
    console.log('Text input focus lost:', text);
  };

  getHeightAndWidth = (height: number, width: number) => {
    console.log(' onContentSizeChange height: ' + height + ' width: ' + width);
  };

  render() {
    let textInputRef: TextInput | null;
    return (
      <TouchableWithoutFeedback>
        <ScrollView>
          <View style={styles.container}>
            <TextInput
              style={styles.input}
              placeholder={'MultiLine'}
              multiline={true}
              aria-level={1}
              aria-posinset={1}
              aria-setsize={30}
            />
            <TextInput
              style={[styles.input, {maxHeight: 120}]}
              multiline={true}
              onContentSizeChange={event =>
                this.getHeightAndWidth(
                  event.nativeEvent.contentSize.height,
                  event.nativeEvent.contentSize.width,
                )
              }
              placeholder={'MultiLine - content size change'}
            />
            <TextInput
              style={styles.input}
              placeholder={'ReadOnly'}
              editable={false}
            />
            <TextInput
              style={styles.input}
              selectTextOnFocus={true}
              placeholder={'Select text on focus'}
            />
            <TextInput
              style={styles.input}
              clearTextOnFocus={true}
              selectTextOnFocus={true}
              placeholder={
                'Clear text on focus, even if selectTextOnFocus is true'
              }
            />
            <TextInput
              style={styles.input}
              placeholder={'SpellChecking Enabled Autocorrect Disabled'}
              spellCheck={true}
              autoCorrect={false}
            />
            <TextInput
              style={styles.input}
              placeholder={'SpellChecking Enabled Autocorrect Enabled'}
              spellCheck={true}
              autoCorrect={true}
            />
            <TextInput
              style={styles.input}
              placeholder={'SpellChecking Disabled Autocorrect Disabled'}
              spellCheck={false}
              autoCorrect={false}
            />
            <TextInput
              style={styles.input}
              placeholder={'SpellChecking Disabled Autocorrect Enabled'}
              spellCheck={false}
              autoCorrect={true}
            />
            <TextInput
              style={styles.input}
              placeholder={
                'SpellChecking default (true) Autocorrect default (true)'
              }
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
              textAlign={'left'}
              autoCapitalize="none"
              placeholder={'Text aligned to the left'}
            />
            <TextInput
              style={styles.input}
              textAlign={'center'}
              autoCapitalize="none"
              placeholder={'Text aligned to the center'}
            />
            <TextInput
              style={styles.input}
              textAlign={'right'}
              autoCapitalize="none"
              placeholder={'Text aligned to the right'}
            />
            <TextInput
              // @ts-ignore
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
            <TextInput
              style={styles.input}
              clearTextOnFocus={true}
              placeholder={'Clear text on focus'}
            />
            <TextInput
              style={[styles.input, {letterSpacing: 5.1}]}
              placeholder={'Letter Spacing'}
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
              style={styles.input}
              placeholder="OnPressIn..."
              onPressIn={event => {
                Alert.alert('Pressed!');
              }}
            />
            <TextInput
              style={styles.input}
              placeholder="OnPressOut..."
              onPressOut={event => {
                Alert.alert('Released!');
              }}
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
            <TextInput
              style={[styles.input, {borderColor: 'red'}]}
              placeholder="Focus and then click outside"
              onEndEditing={this.handleEndEditing}
            />
            <TextInput
              style={[styles.input, {borderColor: 'green', borderWidth: 2}]}
              placeholder={
                'autoFocus: true - This input will be focused automatically'
              }
              autoFocus={true}
            />
            <TextInput
              style={[styles.input, {borderColor: 'red', borderWidth: 2}]}
              placeholder={
                'autoFocus: false - This input will NOT be focused automatically'
              }
              autoFocus={false}
            />
            <TextInput
              style={[
                styles.input,
                {height: 50, borderColor: '#bd1cddff', borderWidth: 2},
              ]}
              multiline={true}
              placeholder="onScroll..."
              scrollEnabled={true}
              onScroll={event => {
                Alert.alert(
                  `onScroll event: ${JSON.stringify(
                    event.nativeEvent.contentOffset,
                  )}`,
                );
              }}
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
      </TouchableWithoutFeedback>
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
