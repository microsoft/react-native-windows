/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Button} from 'react-native';
import {Alert} from '../index.uwp';

class AlertExample extends React.Component {
  _handleClick() {
    Alert.showAlert();
  }

  render() {
    return (
      <View style={{flex: 1}}>
        <Text style={{fontSize: 11}}>Click Button to call alert</Text>
        <Button onPress={this._handleClick} title={'Show Alert.'} />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = '<Alert>';
export const description = 'Example test for the alert module';
export const examples = [
  {
    title: 'Alert',
    render: function(): JSX.Element {
      return <AlertExample />;
    },
  },
];
