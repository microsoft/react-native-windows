/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  Button,
  findNodeHandle,
  requireNativeComponent,
  StyleSheet,
  Text,
  UIManager,
  View,
} from 'react-native';
import { NativeModules } from 'react-native';

const BatchedBridge = require('BatchedBridge');
const NativeEventEmitter = require('NativeEventEmitter');

const CustomUserControlCS = requireNativeComponent('CustomUserControlCS');

const CustomUserControlCPP = requireNativeComponent('CustomUserControlCPP');

var log = function(result) {
  console.log(result);
  NativeModules.DebugConsole.Log('' + result);
};

var getCallback = function(prefix) {
  return function(result) {
    log(prefix + result);
  };
};

class SampleApp extends Component {
  constructor(props) {
    super(props);
    this._cuccsRef = React.createRef();

    // Starting a timer to fire a timed event
    NativeModules.SampleModuleCS.StartTimedEvent(5000, (timerId) =>
    {
      // Registering a handler for SampleModuleCS.TimedEvent event
      BatchedBridge.registerLazyCallableModule('SampleModuleCS', () => {
        const myModuleEventEmitter = new NativeEventEmitter(NativeModules.SampleModuleCS);
        myModuleEventEmitter.addListener('TimedEvent', getCallback(`SampleModuleCS.TimedEvent(${timerId}) => `), this);
        return myModuleEventEmitter;
      });
    });

    // Starting a timer to fire a timed event
    NativeModules.SampleModuleCPP.StartTimedEvent(5000, (timerId) =>
    {
      // Registering a handler for SampleModuleCPP.TimedEvent event
      BatchedBridge.registerLazyCallableModule('SampleModuleCPP', () => {
        const myModuleEventEmitter = new NativeEventEmitter(NativeModules.SampleModuleCPP);
        myModuleEventEmitter.addListener('TimedEvent', getCallback(`SampleModuleCPP.TimedEvent(${timerId}) => `), this);
        return myModuleEventEmitter;
      });
    });
  }

  _onPressHandlerSMCS() {
    log('SampleApp._onPressHandlerSMCS()');

    var numberArg = 42;

    // SampleModuleCS constants

    log(`SampleModuleCS.NumberConstant: ${NativeModules.SampleModuleCS.NumberConstant}`);
    log(`SampleModuleCS.StringConstant: ${NativeModules.SampleModuleCS.StringConstant}`);

    log(`SampleModuleCS.NumberConstantViaProvider: ${NativeModules.SampleModuleCS.NumberConstantViaProvider}`);
    log(`SampleModuleCS.StringConstantViaProvider: ${NativeModules.SampleModuleCS.StringConstantViaProvider}`);

    // SampleModuleCS method calls

    NativeModules.SampleModuleCS.VoidMethod();

    NativeModules.SampleModuleCS.VoidMethodWithArgs(numberArg);

    NativeModules.SampleModuleCS.ReturnMethod(getCallback('SampleModuleCS.ReturnMethod => '));

    NativeModules.SampleModuleCS.ReturnMethodWithArgs(numberArg, getCallback('SampleModuleCS.ReturnMethodWithArgs => '));

    NativeModules.SampleModuleCS.ExplicitCallbackMethod(getCallback('SampleModuleCS.ExplicitCallbackMethod => '));

    NativeModules.SampleModuleCS.ExplicitCallbackMethodWithArgs(numberArg, getCallback('SampleModuleCS.ExplicitCallbackMethodWithArgs => '));

    var promise1 = NativeModules.SampleModuleCS.ExplicitPromiseMethod();
    promise1.then(getCallback('SampleModuleCS.ExplicitPromiseMethod then => ')).catch(getCallback('SampleModuleCS.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCS.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs then => ')).catch(getCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs catch => '));

    log('SampleModuleCS.SyncReturnMethod => ' + NativeModules.SampleModuleCS.SyncReturnMethod());

    log('SampleModuleCS.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCS.SyncReturnMethodWithArgs(numberArg));
  }

  _onPressHandlerSMCPP() {
    log('SampleApp._onPressHandlerSMCPP()');

    var numberArg = 42;

    // SampleModuleCPP constants

    log(`SampleModuleCPP.NumberConstant: ${NativeModules.SampleModuleCPP.NumberConstant}`);
    log(`SampleModuleCPP.StringConstant: ${NativeModules.SampleModuleCPP.StringConstant}`);

    log(`SampleModuleCPP.NumberConstantViaProvider: ${NativeModules.SampleModuleCPP.NumberConstantViaProvider}`);
    log(`SampleModuleCPP.StringConstantViaProvider: ${NativeModules.SampleModuleCPP.StringConstantViaProvider}`);

    // SampleModuleCPP method calls

    NativeModules.SampleModuleCPP.VoidMethod();

    NativeModules.SampleModuleCPP.VoidMethodWithArgs(numberArg);

    NativeModules.SampleModuleCPP.ReturnMethod(getCallback('SampleModuleCPP.ReturnMethod => '));

    NativeModules.SampleModuleCPP.ReturnMethodWithArgs(numberArg, getCallback('SampleModuleCPP.ReturnMethodWithArgs => '));

    NativeModules.SampleModuleCPP.ExplicitCallbackMethod(getCallback('SampleModuleCPP.ExplicitCallbackMethod => '));

    NativeModules.SampleModuleCPP.ExplicitCallbackMethodWithArgs(numberArg, getCallback('SampleModuleCPP.ExplicitCallbackMethodWithArgs => '));

    var promise1 = NativeModules.SampleModuleCPP.ExplicitPromiseMethod();
    promise1.then(getCallback('SampleModuleCPP.ExplicitPromiseMethod then => ')).catch(getCallback('SampleModuleCPP.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCPP.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCPP.ExplicitPromiseMethodWithArgs then => ')).catch(getCallback('SampleModuleCPP.ExplicitPromiseMethodWithArgs catch => '));

    log('SampleModuleCPP.SyncReturnMethod => ' + NativeModules.SampleModuleCPP.SyncReturnMethod());

    log('SampleModuleCPP.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCPP.SyncReturnMethodWithArgs(numberArg));
  }

  _onPressHandlerCUCCS() {
    log('SampleApp._onPressHandlerCUCCS()');

    if (this._cuccsRef)
    {
      const cuccsTag = findNodeHandle(this._cuccsRef);
      log(`tag: ${cuccsTag}`);
      UIManager.dispatchViewManagerCommand(cuccsTag, UIManager.CustomUserControlCS.Commands.CustomCommand, ['Hello World!']);
    }
  }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          SampleApp
        </Text>
        <Text style={styles.instructions}>
          This app consumes custom Native Modules and View Managers.
        </Text>

        <Button onPress={() => { this._onPressHandlerSMCS(); }} title="Call SampleModuleCS!" disabled={NativeModules.SampleModuleCS == null} />
        <Button onPress={() => { this._onPressHandlerSMCPP(); }} title="Call SampleModuleCPP!" disabled={NativeModules.SampleModuleCPP == null} />

        <CustomUserControlCS style={styles.customcontrol} label="CustomUserControlCS!" ref={(ref) => { this._cuccsRef = ref; }} />
        <Button onPress={() => { this._onPressHandlerCUCCS(); }} title="Call CustomUserControlCS Commands!" />

        <CustomUserControlCPP style={styles.customcontrol} label="CustomUserControlCPP!" />
        <Text style={styles.instructions}>
          Hello from Microsoft!
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
  customcontrol: {
    color: '#333333',
    backgroundColor: '#006666',
    width: 200,
    height: 20,
    margin: 10,
  },
});

AppRegistry.registerComponent('SampleApp', () => SampleApp);
