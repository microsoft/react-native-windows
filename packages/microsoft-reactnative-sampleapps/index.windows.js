/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  Alert,
  Button,
  StyleSheet,
  Text,
  View,
} from 'react-native';

import { NativeModules } from 'react-native';

class SampleApp extends Component {
    _onPressHandler() {
        // Simple fire and forget method
        NativeModules.SampleModule.method1();

        // Method with a callback
        NativeModules.SampleModule.method2(
               /* input */ 42,
               /* callback */ function(arg1, arg2, arg3, arg4, arg5) {
                   console.log(arg1);
                   console.log(arg2);
                   console.log(arg3);
                   console.log(arg4);
                   console.log(arg5);
               });

        // Another callback
        NativeModules.SampleModule.method3(
           'foo',
           function(json) {
               console.log(`called back: ${json.result}`);
           });

        // FancyMath Callback
        NativeModules.FancyMath.add(
          /* args */ NativeModules.FancyMath.Pi, NativeModules.FancyMath.E,
          /* callback */ function(result) {
            Alert.alert(
              'FancyMath',
              `FancyMath says ${NativeModules.FancyMath.Pi} + ${NativeModules.FancyMath.E} = ${result}`,
              [
                {
                  text: 'OK',
                },
              ],
              {cancelable: false},
            );
          }
        );

        // An async method that returns a Promise
        var promise = NativeModules.SampleModule.method4(84);

        // log the result
        promise.then(function(result) { console.log(result); });
    }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          Welcome to React Native!
        </Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Button onPress={this._onPressHandler} title="Click me!"/>
        <Text style={styles.instructions}>
          FancyMath says PI = {NativeModules.FancyMath.Pi}
        </Text>
        <Text style={styles.instructions}>
          FancyMath says E = {NativeModules.FancyMath.E}
        </Text>
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
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
});

AppRegistry.registerComponent('SampleApp', () => SampleApp);
