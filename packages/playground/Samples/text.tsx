/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {AppRegistry, StyleSheet} from 'react-native';
import {Text, View} from 'react-native-windows';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>Welcome to React Native!</Text>
        <Text
          style={styles.welcome}
          tooltip=" tooltip message for the text"
          accessible={true}
          selectable={true}>
          Click here : This is a text with a tooltip.
        </Text>
        <View
          style={styles.container2}
          accessible={true}
          accessibilityLabel="Annotation Checkc"
          accessibilityAnnotation={{
            typeID: 'Comment',
            typeName: 'Check Comment',
            author: 'Christopher tarantino',
            dateTime: '3/19/2025 1:03 PM',
          }}>
          <Text
            adjustsFontSizeToFit
            style={{maxHeight: 80, fontSize: 72}}
            minimumFontScale={0.5}>
            Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
            eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim
            ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut
            aliquip ex ea commodo consequat.
          </Text>
        </View>

        {/* Writing Direction Examples */}
        <View style={styles.writingDirectionContainer}>
          <Text style={styles.sectionTitle}>Writing Direction Examples</Text>

          <Text style={styles.directionLabel}>Left-to-Right (LTR):</Text>
          <Text
            style={styles.directionExample}
            {...({writingDirection: 'ltr'} as any)}>
            This text flows from left to right. Numbers: 123 456 789
          </Text>

          <Text style={styles.directionLabel}>Right-to-Left (RTL):</Text>
          <Text
            style={styles.directionExample}
            {...({writingDirection: 'rtl'} as any)}>
            This text flows from right to left. Numbers: 123 456 789
          </Text>

          <Text style={styles.directionLabel}>Auto/Natural:</Text>
          <Text
            style={styles.directionExample}
            {...({writingDirection: 'auto'} as any)}>
            This text uses natural direction (defaults to LTR)
          </Text>
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#C5CCFF',
  },
  container2: {
    backgroundColor: 'lightcoral',
    padding: 10,
    marginBottom: 10,
    width: 500,
    height: 100,
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  writingDirectionContainer: {
    backgroundColor: '#E8F4FD',
    padding: 15,
    margin: 10,
    borderRadius: 8,
    width: 400,
  },
  sectionTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 15,
    textAlign: 'center',
    color: '#2C3E50',
  },
  directionLabel: {
    fontSize: 14,
    fontWeight: '600',
    marginTop: 10,
    marginBottom: 5,
    color: '#34495E',
  },
  directionExample: {
    fontSize: 16,
    padding: 8,
    backgroundColor: '#FFFFFF',
    borderRadius: 4,
    borderWidth: 1,
    borderColor: '#BDC3C7',
    marginBottom: 8,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
