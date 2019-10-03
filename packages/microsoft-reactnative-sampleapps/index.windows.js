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
  _onPressHandlerSM() {
    var numberArg = 42;

    var logCallback = function(result) {
      console.log(result);
    };

    // SampleModule constants

    logCallback(`SampleModule.NumberConstant: ${NativeModules.SampleModule.NumberConstant}`);
    logCallback(`SampleModule.StringConstant: ${NativeModules.SampleModule.StringConstant}`);

    // SampleModule method calls

    NativeModules.SampleModule.VoidMethod();

    NativeModules.SampleModule.VoidMethodWithArgs(numberArg);

    NativeModules.SampleModule.ReturnMethod(logCallback);

    NativeModules.SampleModule.ReturnMethodWithArgs(numberArg, logCallback);

    NativeModules.SampleModule.ExplicitVoidMethod();

    NativeModules.SampleModule.ExplicitVoidMethodWithArgs(numberArg);

    NativeModules.SampleModule.ExplicitCallbackMethod(logCallback);

    NativeModules.SampleModule.ExplicitCallbackMethodWithArgs(numberArg, logCallback);

    var promise1 = NativeModules.SampleModule.ExplicitPromiseMethod();
    promise1.then(logCallback, logCallback);

    var promise2 = NativeModules.SampleModule.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(logCallback, logCallback);
  }

  _onPressHandlerFM() {
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
  }

  _onPressHandlerSMA() {
    var numberArg = 42;

    var logCallback = function(result) {
      console.log(result);
    };

    // SampleModuleABI constants

    logCallback(`SampleModuleABI.NumberConstant: ${NativeModules.SampleModuleABI.NumberConstant}`);
    logCallback(`SampleModuleABI.StringConstant: ${NativeModules.SampleModuleABI.StringConstant}`);

    // SampleModuleABI method calls

    NativeModules.SampleModuleABI.VoidMethod();

    NativeModules.SampleModuleABI.VoidMethodWithArgs(numberArg);

    NativeModules.SampleModuleABI.ReturnMethod(logCallback);

    NativeModules.SampleModuleABI.ReturnMethodWithArgs(numberArg, logCallback);

    NativeModules.SampleModuleABI.ExplicitVoidMethod();

    NativeModules.SampleModuleABI.ExplicitVoidMethodWithArgs(numberArg);

    NativeModules.SampleModuleABI.ExplicitCallbackMethod(logCallback);

    NativeModules.SampleModuleABI.ExplicitCallbackMethodWithArgs(numberArg, logCallback);

    var promise1 = NativeModules.SampleModuleABI.ExplicitPromiseMethod();
    promise1.then(logCallback, logCallback);

    var promise2 = NativeModules.SampleModuleABI.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(logCallback, logCallback);
  }

  _onPressHandlerFMA() {
    // FancyMathABI Callback
    NativeModules.FancyMathABI.add(
      /* args */ NativeModules.FancyMathABI.Pi, NativeModules.FancyMathABI.E,
      /* callback */ function(result) {
        Alert.alert(
          'FancyMathABI',
          `FancyMathABI says ${NativeModules.FancyMathABI.Pi} + ${NativeModules.FancyMathABI.E} = ${result}`,
          [
            {
              text: 'OK',
            },
          ],
          {cancelable: false},
        );
      }
    );
  }

  _onPressHandlerCppCalculator() {
    var getCallback = function(prefix) {
      return function(result) {
        console.log(prefix + result);
        NativeModules.DebugConsole.Log(prefix + result + "\n");
      };
    }

    getCallback("")("Hello from JS!");

    NativeModules.Calculator.Add(5, 6,
      getCallback("Calculator.Add(5, 6) => "));
    NativeModules.Calculator.Add(5, 12,
      getCallback("Calculator.Add(5, 12) => "));
    NativeModules.Calculator.Subtract(6, 5)
      .then(getCallback("Calculator.Subtract(6, 5) => then "))
      .catch(getCallback("Calculator.Subtract(6, 5) => catch "));
    NativeModules.Calculator.Subtract(5, 6)
      .then(getCallback("Calculator.Subtract(5, 6) => then "))
      .catch(getCallback("Calculator.Subtract(5, 6) => catch "));

    NativeModules.CsStrings.Length("Hello!",
      getCallback("CsStrings.Length(\"Hello!\") => "));
    NativeModules.CsStrings.Concat("Hello", "World!",
      getCallback("CsStrings.Concat(\"Hello\", \"World!\") => "));
    NativeModules.CsStrings.Substr("Hello World!", 5)
      .then(getCallback("CsStrings.Substr(\"Hello World!\", 5) => then "))
      .catch(getCallback("CsStrings.Substr(\"Hello World!\", 5) => catch "));
    NativeModules.CsStrings.Substr("Hello World!", 20)
      .then(getCallback("CsStrings.Substr(\"Hello World!\", 20) => then "))
      .catch(getCallback("CsStrings.Substr(\"Hello World!\", 20) => catch "));
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

        <Button onPress={this._onPressHandlerSM} title="Call SampleModule!"/>
        <Button onPress={this._onPressHandlerFM} title="Call FancyMath!"/>

        <Button onPress={this._onPressHandlerSMA} title="Call SampleModuleABI!"/>
        <Button onPress={this._onPressHandlerFMA} title="Call FancyMathABI!"/>
       
        <Button onPress={this._onPressHandlerCppCalculator} title="Call SampleLibraryCPP Calculator"/>
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
