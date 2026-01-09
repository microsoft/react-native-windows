/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  AppRegistry,
  StyleSheet,
  ScrollView,
  Text,
  View,
  TextInput,
} from 'react-native';

class KeyboardTypeTest extends React.Component<
  {},
  {
    defaultValue: string;
    numericValue: string;
    numberPadValue: string;
    decimalPadValue: string;
    emailValue: string;
    phonePadValue: string;
    urlValue: string;
    webSearchValue: string;
    secureNumericValue: string;
  }
> {
  constructor(props: {}) {
    super(props);
    this.state = {
      defaultValue: '',
      numericValue: '',
      numberPadValue: '',
      decimalPadValue: '',
      emailValue: '',
      phonePadValue: '',
      urlValue: '',
      webSearchValue: '',
      secureNumericValue: '',
    };
  }

  render() {
    return (
      <ScrollView style={styles.container}>
        <Text style={styles.title}>Keyboard Type Test (Fabric)</Text>
        <Text style={styles.subtitle}>Test SetInputScopes on Parent HWND</Text>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Default Keyboard:</Text>
          <TextInput
            style={styles.input}
            keyboardType="default"
            value={this.state.defaultValue}
            onChangeText={text => this.setState({defaultValue: text})}
            placeholder="default keyboard"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Numeric Keyboard:</Text>
          <TextInput
            style={styles.input}
            keyboardType="numeric"
            value={this.state.numericValue}
            onChangeText={text => this.setState({numericValue: text})}
            placeholder="numeric keyboard"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Number Pad:</Text>
          <TextInput
            style={styles.input}
            keyboardType="number-pad"
            value={this.state.numberPadValue}
            onChangeText={text => this.setState({numberPadValue: text})}
            placeholder="number-pad"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Decimal Pad:</Text>
          <TextInput
            style={styles.input}
            keyboardType="decimal-pad"
            value={this.state.decimalPadValue}
            onChangeText={text => this.setState({decimalPadValue: text})}
            placeholder="decimal-pad"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Email Address:</Text>
          <TextInput
            style={styles.input}
            keyboardType="email-address"
            value={this.state.emailValue}
            onChangeText={text => this.setState({emailValue: text})}
            placeholder="email-address"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Phone Pad:</Text>
          <TextInput
            style={styles.input}
            keyboardType="phone-pad"
            value={this.state.phonePadValue}
            onChangeText={text => this.setState({phonePadValue: text})}
            placeholder="phone-pad"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>URL Keyboard:</Text>
          <TextInput
            style={styles.input}
            keyboardType="url"
            value={this.state.urlValue}
            onChangeText={text => this.setState({urlValue: text})}
            placeholder="url"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Web Search:</Text>
          <TextInput
            style={styles.input}
            keyboardType="web-search"
            value={this.state.webSearchValue}
            onChangeText={text => this.setState({webSearchValue: text})}
            placeholder="web-search"
          />
        </View>

        <View style={styles.inputContainer}>
          <Text style={styles.label}>Secure + Numeric:</Text>
          <TextInput
            style={styles.input}
            keyboardType="numeric"
            secureTextEntry={true}
            value={this.state.secureNumericValue}
            onChangeText={text => this.setState({secureNumericValue: text})}
            placeholder="numeric password"
          />
        </View>

        <View style={styles.instructions}>
          <Text style={styles.instructionText}>
            Instructions for Testing on Windows:{'\n'}
            {'\n'}
            This test uses SetInputScopes on the parent HWND.{'\n'}
            {'\n'}
            To test with Windows Touch Keyboard:{'\n'}
            1. Right-click taskbar → Show touch keyboard button{'\n'}
            2. Click the keyboard icon in system tray{'\n'}
            3. Tap/click each TextInput field to focus it{'\n'}
            4. Observe the touch keyboard layout changes{'\n'}
            {'\n'}
            Expected keyboard layouts:{'\n'}• default: Standard QWERTY{'\n'}•
            numeric/number-pad: Number keys (IS_NUMBER/IS_DIGITS){'\n'}•
            decimal-pad: Numbers with decimal point{'\n'}• email-address: QWERTY
            with @ and .com keys{'\n'}• phone-pad: Phone dial pad layout{'\n'}•
            url: QWERTY with .com/.net buttons{'\n'}• web-search:
            Search-optimized layout{'\n'}• secure+numeric: PIN entry layout
            {'\n'}
            {'\n'}
            Note: Physical keyboard allows all input (matches iOS/Android).
          </Text>
        </View>
      </ScrollView>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    backgroundColor: '#f5f5f5',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 5,
    color: '#333',
  },
  subtitle: {
    fontSize: 14,
    marginBottom: 20,
    color: '#666',
    fontStyle: 'italic',
  },
  inputContainer: {
    marginBottom: 15,
  },
  label: {
    fontSize: 14,
    fontWeight: '600',
    marginBottom: 5,
    color: '#444',
  },
  input: {
    borderWidth: 1,
    borderColor: '#ccc',
    borderRadius: 4,
    padding: 10,
    fontSize: 16,
    backgroundColor: '#fff',
  },
  instructions: {
    marginTop: 30,
    padding: 15,
    backgroundColor: '#e3f2fd',
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#90caf9',
  },
  instructionText: {
    fontSize: 13,
    color: '#1565c0',
    lineHeight: 20,
  },
});

AppRegistry.registerComponent('KeyboardTypeTest', () => KeyboardTypeTest);
