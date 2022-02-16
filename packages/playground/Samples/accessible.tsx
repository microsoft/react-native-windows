/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TextInput,
  View,
  AccessibilityInfo,
  findNodeHandle,
  TouchableHighlight,
} from 'react-native';
import {ViewWindows} from 'react-native-windows';

export default class Bootstrap extends React.Component<
  {},
  {displayText: string}
> {
  constructor(props: {}) {
    super(props);
    this.state = {displayText: 'Starting text. (THIRD ITEM)'};
  }

  myElement = React.createRef<TextInput>();

  render() {
    return (
      <View style={styles.container}>
        <View style={styles.item} accessibilityLabel="FIRST ITEM" focusable>
          <Text style={styles.text}>Welcome to React Native! (FIRST ITEM)</Text>
        </View>
        <ViewWindows
          style={styles.item}
          accessibilityLabel="SECOND ITEM"
          focusable
          enableFocusRing={false}>
          <Text style={styles.text}>No focus visual (SECOND ITEM)</Text>
        </ViewWindows>
        <ViewWindows
          style={styles.item}
          accessibilityLabel="THIRD ITEM"
          focusable
          enableFocusRing={false}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            onFocus: () => this.setState({displayText: 'FOCUSED'}),
            onBlur: () => this.setState({displayText: 'BLURRED'}),
          }}>
          <Text style={styles.text}>{this.state.displayText}</Text>
        </ViewWindows>
        <TouchableHighlight
          style={styles.item}
          accessibilityLabel="TEST Announce For Accessibility"
          onPress={() => {
            AccessibilityInfo.announceForAccessibility('Testing Testing 1 2 3');
          }}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            focusable: true,
          }}>
          <Text style={styles.text}>TEST announceForAccessibility</Text>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.item}
          accessibilityLabel="TEST Set Accessibility Focus"
          onPress={() => {
            const reactTag = findNodeHandle(this.myElement.current);
            if (reactTag) {
              AccessibilityInfo.setAccessibilityFocus(reactTag);
            }
          }}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            focusable: true,
          }}>
          <Text style={styles.text}>TEST setAccessibilityFocus</Text>
        </TouchableHighlight>
        <TextInput ref={this.myElement} />
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
