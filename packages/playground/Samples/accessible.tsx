/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, StyleSheet, Text, TextInput, View} from 'react-native';

export default class Bootstrap extends React.Component<
  {},
  {displayText: string}
> {
  constructor(props: {}) {
    super(props);
    this.state = {displayText: 'Starting text. (THIRD ITEM)'};
  }

  render() {
    return (
      <View style={styles.container}>
        <View
          style={styles.item}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            acceptsKeyboardFocus: true,
            accessibilityLabel: 'FIRST ITEM',
          }}>
          <Text style={styles.text}>Welcome to React Native! (FIRST ITEM)</Text>
        </View>
        <View
          style={styles.item}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            enableFocusRing: false,
            acceptsKeyboardFocus: true,
            accessibilityLabel: 'SECOND ITEM',
          }}>
          <Text style={styles.text}>No focus visual (SECOND ITEM)</Text>
        </View>
        <View
          style={styles.item}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            onFocus: () => this.setState({displayText: 'FOCUSED'}),
            onBlur: () => this.setState({displayText: 'BLURRED'}),
            enableFocusRing: false,
            acceptsKeyboardFocus: true,
            accessibilityLabel: 'THIRD ITEM',
          }}>
          <Text style={styles.text}>{this.state.displayText}</Text>
        </View>
        <TextInput />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'lightgray',
  },
  item: {
    margin: 10,
    backgroundColor: 'lightpink',
    borderColor: 'indianred',
    borderWidth: 2,
    justifyContent: 'center',
  },
  text: {
    fontSize: 20,
    textAlign: 'center',
    color: 'black',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
