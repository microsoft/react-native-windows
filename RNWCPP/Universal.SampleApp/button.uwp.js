// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

import React, { Component } from 'react';
import {
  AppRegistry,
  Text,
  TouchableHighlight,
  View
} from 'react-native';

export default class Bootstrap extends Component {
  render() {
    return (
      <View style={{ flex: 1, justifyContent: 'center', alignItems: 'center', backgroundColor: 'blue' }}>
        <View style={{ backgroundColor: 'white' }}>
          <Text style={{ textAlign: 'center', color: 'black', padding: 10 }}>
            Button!
          </Text>
        </View>
        <View style={{ height: 15 }} />
        <View style={{ backgroundColor: 'white' }}>
          <View style={{ backgroundColor: 'green', padding: 10 }}>
            <View style={{ backgroundColor: 'orange', width: 60, height: 20,  }}/>
          </View>
        </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
