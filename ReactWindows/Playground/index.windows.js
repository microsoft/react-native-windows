/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TouchableHighlight,
  View
} from 'react-native';

var Playground = React.createClass({
  getInitialState: function() {
    return {fontSize: 20};
  },
  
  render: function() {
    var curStyle = {fontSize: this.state.fontSize};
    return (
      <View style={styles.container}>
        <Text style={[styles.welcome, curStyle]}>
          Welcome to 
          <TouchableHighlight
            style={styles.inline}
            onPress={this.onPress}>
            <View style={styles.innerInline} />
          </TouchableHighlight>
          React Native!
        </Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Text style={styles.instructions}>
          Shake or press Shift+F10 for dev menu
        </Text>
      </View>
    );
  },

  onPress: function() {
    this.setState({
      fontSize: this.state.fontSize + 1,
    });
  }
});

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
  inline: {
    width: 50,
    height: 50,
    backgroundColor: 'red', 
  },
  innerInline: {
    width: 20,
    height: 20,
    margin: 15,
    backgroundColor: 'blue', 
  }
});

AppRegistry.registerComponent('Playground', () => Playground);
