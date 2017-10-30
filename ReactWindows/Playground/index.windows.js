/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  Button,
  NativeModules,
  StyleSheet,
  Text,
  View,
} from 'react-native';

let _ = require('AppBackgroundModule');

class Playground extends Component {
  state = {
    registrationKey: false,
  }

  onPress() {
    if (this.state.registrationKey) {
      NativeModules.Background.unregister(this.state.registrationKey);
      this.setState({registrationKey: null});   
    } else {
      NativeModules.Background.registerSystemTrigger("test").then(key => {
        this.setState({registrationKey: key});
      });      
    }
  }

  render() {
    let buttonTitle = this.state.registrationKey ? "Unregister" : "Register";
    return (
      <View style={styles.container}>
        <Button title={buttonTitle} onPress={this.onPress.bind(this)} />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
});

AppRegistry.registerComponent('Playground', () => Playground);
