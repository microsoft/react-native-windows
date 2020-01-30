/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Alert, TouchableHighlight, StyleSheet} from 'react-native';

const alertMessage =
  'Credibly reintermediate next-generation potentialities after goal-oriented ' +
  'catalysts for change. Dynamically revolutionize.';

class AlertExamples extends React.Component {
  public render() {
    return (
      <View>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() => Alert.alert('Alert Title', alertMessage)}>
          <View style={styles.button}>
            <Text>Alert with message and default button</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('Alert Title', alertMessage, [
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ])
          }>
          <View style={styles.button}>
            <Text>Alert with one button</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('Alert Title', alertMessage, [
              {text: 'Cancel', onPress: () => console.log('Cancel Pressed!')},
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ])
          }>
          <View style={styles.button}>
            <Text>Alert with two buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('Alert Title', undefined, [
              {text: 'Foo', onPress: () => console.log('Foo Pressed!')},
              {text: 'Bar', onPress: () => console.log('Bar Pressed!')},
              {text: 'Baz', onPress: () => console.log('Baz Pressed!')},
            ])
          }>
          <View style={styles.button}>
            <Text>Alert with three buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert(
              'Foo Title',
              alertMessage,
              '..............'.split('').map((dot, index) => ({
                text: 'Button ' + index,
                onPress: () => console.log('Pressed ' + index),
              })),
            )
          }>
          <View style={styles.button}>
            <Text>Alert with too many buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert(
              'Alert Title',
              undefined,
              [{text: 'OK', onPress: () => console.log('OK Pressed!')}],
              {
                cancelable: false,
              },
            )
          }>
          <View style={styles.button}>
            <Text>Alert that cannot be dismissed</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('', alertMessage, [
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ])
          }>
          <View style={styles.button}>
            <Text>Alert without title</Text>
          </View>
        </TouchableHighlight>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  wrapper: {
    borderRadius: 5,
    marginBottom: 5,
  },
  button: {
    backgroundColor: '#eeeeee',
    padding: 10,
  },
});

export const displayName = (_undefined?: string) => {};
export const title = 'Alert';
export const description =
  'Alerts display a concise and informative message ' +
  'and prompt the user to make a decision.';
export const examples = [
  {
    title: 'Alert examples',
    render: function(): JSX.Element {
      return <AlertExamples />;
    },
  },
];
