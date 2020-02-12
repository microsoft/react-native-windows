/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

'use strict';

const React = require('react');

const {Text, View} = require('react-native');

class SetPropertiesExampleApp extends React.Component<$FlowFixMeProps> {
  render(): React.Node {
    const wrapperStyle = {
      backgroundColor: this.props.color,
      flex: 1,
      alignItems: 'center',
      justifyContent: 'center',
    };

    return (
      <View style={wrapperStyle}>
        <Text>Embedded React Native view</Text>
      </View>
    );
  }
}

module.exports = SetPropertiesExampleApp;
