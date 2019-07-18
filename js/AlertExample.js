/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {Alert, StyleSheet, Text, TouchableHighlight, View} = ReactNative;

const RNTesterBlock = require('./RNTesterBlock');

// corporate ipsum > lorem ipsum
const alertMessage =
  'Credibly reintermediate next-generation potentialities after goal-oriented ' +
  'catalysts for change. Dynamically revolutionize.';

/**
 * Simple alert examples.
 */
type Props = $ReadOnly<{||}>;

class SimpleAlertExampleBlock extends React.Component<Props> {
  componentDidMount() {
    // This is basically the tag of the RCTView corresponding to the non-collapsable View child
    this._rootViewHint = ReactNative.findNodeHandle(this);
  }

  _getDefaultOptions() {
    return  {rootViewHint: this._rootViewHint};
  }

  render() {
    return (
      <View collapsable={false}>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() => Alert.alert(
            'Alert Title',
            alertMessage,
            [],
            this._getDefaultOptions()
          )}>
          <View style={styles.button}>
            <Text>Alert with message and default button</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('Alert Title', alertMessage, [
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ],
            this._getDefaultOptions()
          )}>
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
            ],
            this._getDefaultOptions()
          )}>
          <View style={styles.button}>
            <Text>Alert with two buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('Alert Title', null, [
              {text: 'Foo', onPress: () => console.log('Foo Pressed!')},
              {text: 'Bar', onPress: () => console.log('Bar Pressed!')},
              {text: 'Baz', onPress: () => console.log('Baz Pressed!')},
            ],
            this._getDefaultOptions()
          )}>
          <View style={styles.button}>
            <Text>Alert with three buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() => Alert.alert(
            'Foo Title',
            alertMessage,
            '..............'.split('').map((dot, index) => ({
              text: 'Button ' + index,
              onPress: () => console.log('Pressed ' + index)
            })),
            this._getDefaultOptions()
          )}>
          <View style={styles.button}>
            <Text>Alert with too many buttons</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() => Alert.alert(
            'Alert Title',
            null,
            [
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ],
            { ...this._getDefaultOptions(), cancelable: false }
          )}>
          <View style={styles.button}>
            <Text>Alert that cannot be dismissed</Text>
          </View>
        </TouchableHighlight>
        <TouchableHighlight
          style={styles.wrapper}
          onPress={() =>
            Alert.alert('', alertMessage, [
              {text: 'OK', onPress: () => console.log('OK Pressed!')},
            ],
            this._getDefaultOptions()
          )}>
          <View style={styles.button}>
            <Text>Alert without title</Text>
          </View>
        </TouchableHighlight>
      </View>
    );
  }
}

class AlertExample extends React.Component {
  static title = 'Alert';

  static description =
    'Alerts display a concise and informative message ' +
    'and prompt the user to make a decision.';

  render() {
    return (
      <RNTesterBlock title={'Alert'}>
        <SimpleAlertExampleBlock />
      </RNTesterBlock>
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

module.exports = {
  AlertExample,
  SimpleAlertExampleBlock,
};
