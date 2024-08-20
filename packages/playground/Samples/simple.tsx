/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, View, Text} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View>
        <View
          accessibilityLiveRegion="polite"
          aria-live="polite"
          accessibilityLabel="Change Label" accessible>
          <Text>Polite Accessibility</Text>
        </View>
        <View
          accessibilityLiveRegion="assertive"
          aria-live="off"
          accessibilityLabel="None Accessibility" accessible>
          <Text>None Accessibility</Text>
        </View>
        <View
          accessibilityLiveRegion="assertive"
          aria-live="assertive"
          accessibilityLabel="Assertive Accessibility" accessible>
          <Text>Assertive Accessibility</Text>
        </View>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);