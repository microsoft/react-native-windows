/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, StyleSheet, Image, View} from 'react-native';

const styles = StyleSheet.create({
  base: {
    width: 164,
    height: 164,
    margin: 4,
  },
});

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View
        accessible={true}
        style={{borderRadius: 30, width: 600, height: 600, margin: 10}}>
        <View style={{backgroundColor: 'magenta', width: 60, height: 60}} />
        <Image
          source={{
            uri: 'https://www.facebook.com/ads/pics/successstories.png',
          }}
          style={styles.base}
        />
        <Text style={{color: 'green'}}>Hello!</Text>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
