/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Button} from 'react-native';
import {Alert} from '../index.windows';

class AlertExample extends React.Component {
  state = {
    buttonPressed: 'none',
  };

  _threeButtons = () => {
    var _this = this;

    Alert.alert(
      'Alert Title',
      'Place the alert message here',
      [
        {
          text: 'Ask me later',
          onPress: () => _this.setState({buttonPressed: 'Ask me later'}),
        },
        {
          text: 'Cancel',
          onPress: () => _this.setState({buttonPressed: 'Cancel'}),
          style: 'cancel',
        },
        {
          text: 'OK',
          onPress: () => _this.setState({buttonPressed: 'OK'}),
        },
      ],
      {cancelable: false},
    );
  };

  _twoButtons = () => {
    var _this = this;

    Alert.alert(
      'Alert Title',
      'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.',
      [
        {
          text: 'Cancel',
          onPress: () => _this.setState({buttonPressed: 'Cancel'}),
          style: 'cancel',
        },
        {
          text: 'OK',
          onPress: () => _this.setState({buttonPressed: 'OK'}),
        },
      ],
      {cancelable: false},
    );
  };

  _oneButton = () => {
    var _this = this;

    Alert.alert(
      'Alert Title',
      'Place the alert message here',
      [
        {
          text: 'OK',
          onPress: () => _this.setState({buttonPressed: 'OK'}),
        },
      ],
      {cancelable: false},
    );
  };

  render() {
    return (
      <View style={{flex: 1}}>
        <Text style={{fontSize: 14}}>Click Button to call alert</Text>
        <View style={{marginTop: 10}} />
        <Button onPress={this._oneButton} title={'Alert with one button'} />
        <View style={{marginTop: 10}} />
        <Button
          onPress={this._twoButtons}
          title={'Alert with two buttons and larger message'}
        />
        <View style={{marginTop: 10}} />
        <Button
          onPress={this._threeButtons}
          title={'Alert with three buttons'}
        />
        <Text style={{fontSize: 14, marginTop: 10}}>
          Button Pressed: {this.state.buttonPressed}
        </Text>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'Alert';
export const description = 'Example test for the alert module';
export const examples = [
  {
    title: 'Alert',
    render: function(): JSX.Element {
      return <AlertExample />;
    },
  },
];
