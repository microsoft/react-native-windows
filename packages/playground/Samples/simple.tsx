/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
import {AppRegistry, Text, ScrollView, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View style={{borderRadius: 30, width: 60, height: 60, margin: 10}}>
        <ScrollView accessible={true}>
          <Text>Hello</Text>
          <Text>Hello</Text>
          <Text>Hello</Text>
          <View style={{backgroundColor: 'magenta', width: 60, height: 60}} />
        </ScrollView>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
