/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View
        accessible={true}
        style={{borderRadius: 30, width: 60, height: 60, margin: 10}}
      />
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
