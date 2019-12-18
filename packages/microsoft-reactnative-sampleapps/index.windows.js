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

import { NativeModules, NativeEventEmitter } from 'react-native';

// Creating event emitters
const SampleModuleCSEmitter = new NativeEventEmitter(NativeModules.SampleModuleCS);
const SampleModuleCppEmitter = new NativeEventEmitter(NativeModules.SampleModuleCpp);

const CustomUserControlCS = requireNativeComponent('CustomUserControlCS');
const CustomUserControlCpp = requireNativeComponent('CustomUserControlCpp');

const CircleCS = requireNativeComponent('CircleCS');
const CircleCpp = requireNativeComponent('CircleCpp');

var log = function(result) {
  console.log(result);
  NativeModules.DebugConsole.Log('' + result);
};

var getCallback = function(prefix) {
  return function(result) {
    log(prefix + result);
  };
};

var getErrorCallback = function(prefix) {
  return function(error) {
    log(prefix + (error || {}).message);
  };
};

class SampleApp extends Component {
  componentDidMount() {
    this._TimedEventCSSub = SampleModuleCSEmitter.addListener('TimedEventCS', getCallback('SampleModuleCS.TimedEventCS() => '));
    this._TimedEventCppSub = SampleModuleCppEmitter.addListener('TimedEventCpp', getCallback('SampleModuleCpp.TimedEventCpp() => '));
  }

  componentWillUnmount() {
    this._TimedEventCSSub.remove();
    this._TimedEventCppSub.remove();
  }

  onPressSampleModuleCS() {
    log('SampleApp.onPressSampleModuleCS()');

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
    promise1.then(getCallback('SampleModuleCS.ExplicitPromiseMethod then => ')).catch(getErrorCallback('SampleModuleCS.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCS.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs then => ')).catch(getErrorCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs catch => '));

    log('SampleModuleCS.SyncReturnMethod => ' + NativeModules.SampleModuleCS.SyncReturnMethod());

    log('SampleModuleCS.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCS.SyncReturnMethodWithArgs(numberArg));
  }

  onPressSampleModuleCpp() {
    log('SampleApp.onPressSampleModuleCpp()');

    var numberArg = 42;

    // SampleModuleCpp constants

    log(`SampleModuleCpp.NumberConstant: ${NativeModules.SampleModuleCpp.NumberConstant}`);
    log(`SampleModuleCpp.StringConstant: ${NativeModules.SampleModuleCpp.StringConstant}`);

    log(`SampleModuleCpp.NumberConstantViaProvider: ${NativeModules.SampleModuleCpp.NumberConstantViaProvider}`);
    log(`SampleModuleCpp.StringConstantViaProvider: ${NativeModules.SampleModuleCpp.StringConstantViaProvider}`);

    // SampleModuleCpp method calls

    NativeModules.SampleModuleCpp.VoidMethod();

    NativeModules.SampleModuleCpp.VoidMethodWithArgs(numberArg);

    NativeModules.SampleModuleCpp.ReturnMethod(getCallback('SampleModuleCpp.ReturnMethod => '));

    NativeModules.SampleModuleCpp.ReturnMethodWithArgs(numberArg, getCallback('SampleModuleCpp.ReturnMethodWithArgs => '));

    NativeModules.SampleModuleCpp.ExplicitCallbackMethod(getCallback('SampleModuleCpp.ExplicitCallbackMethod => '));

    NativeModules.SampleModuleCpp.ExplicitCallbackMethodWithArgs(numberArg, getCallback('SampleModuleCpp.ExplicitCallbackMethodWithArgs => '));

    var promise1 = NativeModules.SampleModuleCpp.ExplicitPromiseMethod();
    promise1.then(getCallback('SampleModuleCpp.ExplicitPromiseMethod then => ')).catch(getErrorCallback('SampleModuleCpp.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCpp.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCpp.ExplicitPromiseMethodWithArgs then => ')).catch(getErrorCallback('SampleModuleCpp.ExplicitPromiseMethodWithArgs catch => '));

    log('SampleModuleCpp.SyncReturnMethod => ' + NativeModules.SampleModuleCpp.SyncReturnMethod());

    log('SampleModuleCpp.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCpp.SyncReturnMethodWithArgs(numberArg));
  }

  onPressCustomUserControlCS() {
    log('SampleApp.onPressCustomUserControlCS()');

    var strArg = 'Hello World!';

    if (this._CustomUserControlCSRef)
    {
      const tag = findNodeHandle(this._CustomUserControlCSRef);
      log(`UIManager.dispatchViewManagerCommand(${tag}, CustomUserControlCS.CustomCommand, "${strArg}")`);
      UIManager.dispatchViewManagerCommand(tag, UIManager.getViewManagerConfig('CustomUserControlCS').Commands.CustomCommand, strArg);
    }
  }

  onPressCustomUserControlCpp() {
    log('SampleApp.onPressCustomUserControlCpp()');

    var strArg = 'Hello World!';

    if (this._CustomUserControlCppRef)
    {
      const tag = findNodeHandle(this._CustomUserControlCppRef);
      log(`UIManager.dispatchViewManagerCommand(${tag}, CustomUserControlCpp.CustomCommand, "${strArg}")`);
      UIManager.dispatchViewManagerCommand(tag, UIManager.getViewManagerConfig('CustomUserControlCpp').Commands.CustomCommand, strArg);
    }
  }

  onLabelChangedCustomUserControlCS(evt) {
    var label = evt.nativeEvent;
    log(`SampleApp.onLabelChangedCustomUserControlCS("${label}")`);
  }

  onLabelChangedCustomUserControlCpp(evt) {
    var label = evt.nativeEvent;
    log(`SampleApp.onLabelChangedCustomUserControlCpp("${label}")`);
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

        <Button onPress={() => { this.onPressSampleModuleCS(); }} title="Call SampleModuleCS!" disabled={NativeModules.SampleModuleCS == null} />
        <Button onPress={() => { this.onPressSampleModuleCpp(); }} title="Call SampleModuleCpp!" disabled={NativeModules.SampleModuleCpp == null} />

        <CustomUserControlCS style={styles.customcontrol} label="CustomUserControlCS!" ref={(ref) => { this._CustomUserControlCSRef = ref; }} onLabelChanged={(evt) => { this.onLabelChangedCustomUserControlCS(evt); }} />
        <Button onPress={() => { this.onPressCustomUserControlCS(); }} title="Call CustomUserControlCS Commands!" />

        <CustomUserControlCpp style={styles.customcontrol} label="CustomUserControlCpp!" ref={(ref) => { this._CustomUserControlCppRef = ref; }} onLabelChanged={(evt) => { this.onLabelChangedCustomUserControlCpp(evt); }} />
        <Button onPress={() => { this.onPressCustomUserControlCpp(); }} title="Call CustomUserControlCpp Commands!" />

        <CircleCS style={styles.circle}>
          <View style={styles.box}>
            <Text style={styles.boxText}>CircleCS!</Text>
          </View>
        </CircleCS>

        <CircleCpp style={styles.circle}>
          <View style={styles.box}>
            <Text style={styles.boxText}>CircleCpp!</Text>
          </View>
        </CircleCpp>
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
  circle: {
    margin: 10,
  },
  box: {
    backgroundColor: '#006666',
    width: 100,
    height: 100,
    justifyContent: 'center',
    alignItems: 'center',
  },
  boxText: {
    fontSize: 20,
  },
});

AppRegistry.registerComponent('SampleApp', () => SampleApp);
