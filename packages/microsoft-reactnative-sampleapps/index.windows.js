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
  Linking,
} from 'react-native';

import { NativeModules, NativeEventEmitter } from 'react-native';

import {MyComp} from './myComp';

import {default as MyModule} from './NativeMyModule';

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

var getCallback2 = function(prefix) {
  return function(arg1, arg2) {
    log(prefix + "arg1: " + arg1 + " arg2: " + arg2);
  };
};

var getErrorCallback = function(prefix) {
  return function(error) {
    log(prefix + (error || {}).message);
  };
};

// To demo JS function calls we define a class and then register it as a callable module
class SampleModuleCpp {
  calcDistance(point1, point2) {
    log('SampleApp.calcDistance()');
    const distance = Math.hypot(point1.x - point2.x, point1.y - point2.y);
    log(`Distance between (${point1.x}, ${point1.y}) and (${point2.x}, ${point2.y}) is ${distance}`);
  }
}
global.__fbBatchedBridge.registerLazyCallableModule('SampleModuleCpp', () => new SampleModuleCpp());

class SampleApp extends Component {
  componentDidMount() {
    this.timedEventCSSub = SampleModuleCSEmitter.addListener('TimedEventCS', getCallback('SampleModuleCS.TimedEventCS() => '));
    this.timedEventCppSub = SampleModuleCppEmitter.addListener('TimedEventCpp', getCallback('SampleModuleCpp.TimedEventCpp() => '));
    this.EmitJSEvent1CppSub = SampleModuleCppEmitter.addListener('EmitJSEvent1Cpp', getCallback('SampleModuleCpp.EmitJSEvent1Cpp => '));
    this.EmitJSEvent2CppSub = SampleModuleCppEmitter.addListener('EmitJSEvent2Cpp', getCallback2('SampleModuleCpp.EmitJSEvent2Cpp => '));
    this.EmitJSEvent3CppSub = SampleModuleCppEmitter.addListener('EmitJSEvent3Cpp', getCallback2('SampleModuleCpp.EmitJSEvent3Cpp => '));
    this.EmitJSEvent1CSSub = SampleModuleCppEmitter.addListener('EmitJSEvent1CS', getCallback('SampleModuleCS.EmitJSEvent1CS => '));
    this.EmitJSEvent2CSSub = SampleModuleCppEmitter.addListener('EmitJSEvent2CS', getCallback2('SampleModuleCS.EmitJSEvent2CS => '));
    this.EmitJSEvent3CSSub = SampleModuleCppEmitter.addListener('EmitJSEvent3CS', getCallback2('SampleModuleCS.EmitJSEvent3CS => '));
    this.openURLSub = Linking.addListener('url', (event) => log('Open URL => ' + event.url));

    Linking.getInitialURL()
      .then(url => log('Initial URL is: ' + url))
      .catch(err => log('An error occurred: ' + err));
  }

  componentWillUnmount() {
    this.timedEventCSSub.remove();
    this.timedEventCppSub.remove();
    this.EmitJSEvent1CppSub.remove();
    this.EmitJSEvent2CppSub.remove();
    this.EmitJSEvent3CppSub.remove();
    this.EmitJSEvent1CSSub.remove();
    this.EmitJSEvent2CSSub.remove();
    this.EmitJSEvent3CSSub.remove();
    this.openURLSub.remove();
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

    NativeModules.SampleModuleCS.TwoCallbacksMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCS.TwoCallbacksMethod success => '),
      getCallback('SampleModuleCS.TwoCallbacksMethod fail => '));

    NativeModules.SampleModuleCS.TwoCallbacksMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCS.TwoCallbacksMethod success => '),
      getCallback('SampleModuleCS.TwoCallbacksMethod fail => '));

    NativeModules.SampleModuleCS.TwoCallbacksAsyncMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCS.TwoCallbacksAsyncMethod success => '),
      getCallback('SampleModuleCS.TwoCallbacksAsyncMethod fail => '));

    NativeModules.SampleModuleCS.TwoCallbacksAsyncMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCS.TwoCallbacksAsyncMethod success => '),
      getCallback('SampleModuleCS.TwoCallbacksAsyncMethod fail => '));

    NativeModules.SampleModuleCS.ReverseTwoCallbacksMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCS.ReverseTwoCallbacksMethod fail => '),
      getCallback('SampleModuleCS.ReverseTwoCallbacksMethod success => '));

    NativeModules.SampleModuleCS.ReverseTwoCallbacksMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCS.ReverseTwoCallbacksMethod fail => '),
      getCallback('SampleModuleCS.ReverseTwoCallbacksMethod success => '));

    NativeModules.SampleModuleCS.ReverseTwoCallbacksAsyncMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCS.ReverseTwoCallbacksAsyncMethod fail => '),
      getCallback('SampleModuleCS.ReverseTwoCallbacksAsyncMethod success => '));

    NativeModules.SampleModuleCS.ReverseTwoCallbacksAsyncMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCS.ReverseTwoCallbacksAsyncMethod fail => '),
      getCallback('SampleModuleCS.ReverseTwoCallbacksAsyncMethod success => '));

    var promise1 = NativeModules.SampleModuleCS.ExplicitPromiseMethod();
    promise1.then(getCallback('SampleModuleCS.ExplicitPromiseMethod then => ')).catch(getErrorCallback('SampleModuleCS.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCS.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs then => ')).catch(getErrorCallback('SampleModuleCS.ExplicitPromiseMethodWithArgs catch => '));

    var promise3 = NativeModules.SampleModuleCS.NegateAsyncPromise(5);
    promise3.then(getCallback('SampleModuleCS.NegateAsyncPromise then => ')).catch(getErrorCallback('SampleModuleCS.NegateAsyncPromise catch => '));

    var promise4 = NativeModules.SampleModuleCS.NegateAsyncPromise(-5);
    promise4.then(getCallback('SampleModuleCS.NegateAsyncPromise then => ')).catch(getErrorCallback('SampleModuleCS.NegateAsyncPromise catch => '));

    NativeModules.SampleModuleCS.callDistanceFunction({x: 22, y: 23}, {x: 55, y: 65});

    NativeModules.SampleModuleCS.TaskNoArgs()
      .then(getCallback('SampleModuleCS.TaskNoArgs then => '))
      .catch(getErrorCallback('SampleModuleCS.TaskNoArgs catch => '));

    NativeModules.SampleModuleCS.TaskTwoArgs(11, 200)
      .then(getCallback('SampleModuleCS.TaskTwoArgs then => '))
      .catch(getErrorCallback('SampleModuleCS.TaskTwoArgs catch => '));

    NativeModules.SampleModuleCS.TaskOfTNoArgs()
      .then(getCallback('SampleModuleCS.TaskOfTNoArgs then => '))
      .catch(getErrorCallback('SampleModuleCS.TaskOfTNoArgs catch => '));

    NativeModules.SampleModuleCS.TaskOfTTwoArgs(11, 200)
      .then(getCallback('SampleModuleCS.TaskOfTTwoArgs then => '))
      .catch(getErrorCallback('SampleModuleCS.TaskOfTTwoArgs catch => '));

    NativeModules.SampleModuleCS.EmitJSEvent1(43);
    NativeModules.SampleModuleCS.EmitJSEvent2(8, 52);
    NativeModules.SampleModuleCS.EmitJSEvent3(15, 79);

    //TODO: make sync method accessible only in non-web debugger scenarios
    //log('SampleModuleCS.SyncReturnMethod => ' + NativeModules.SampleModuleCS.SyncReturnMethod());
    //log('SampleModuleCS.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCS.SyncReturnMethodWithArgs(numberArg));
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

    NativeModules.SampleModuleCpp.TwoCallbacksMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCpp.TwoCallbacksMethod success => '),
      getCallback('SampleModuleCpp.TwoCallbacksMethod fail => '));

    NativeModules.SampleModuleCpp.TwoCallbacksMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCpp.TwoCallbacksMethod success => '),
      getCallback('SampleModuleCpp.TwoCallbacksMethod fail => '));

    NativeModules.SampleModuleCpp.TwoCallbacksAsyncMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCpp.TwoCallbacksAsyncMethod success => '),
      getCallback('SampleModuleCpp.TwoCallbacksAsyncMethod fail => '));

    NativeModules.SampleModuleCpp.TwoCallbacksAsyncMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCpp.TwoCallbacksAsyncMethod success => '),
      getCallback('SampleModuleCpp.TwoCallbacksAsyncMethod fail => '));

    NativeModules.SampleModuleCpp.ReverseTwoCallbacksMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCpp.ReverseTwoCallbacksMethod fail => '),
      getCallback('SampleModuleCpp.ReverseTwoCallbacksMethod success => '));

    NativeModules.SampleModuleCpp.ReverseTwoCallbacksMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCpp.ReverseTwoCallbacksMethod fail => '),
      getCallback('SampleModuleCpp.ReverseTwoCallbacksMethod success => '));

    NativeModules.SampleModuleCpp.ReverseTwoCallbacksAsyncMethod(/*shouldSucceed:*/true,
      getCallback('SampleModuleCpp.ReverseTwoCallbacksAsyncMethod fail => '),
      getCallback('SampleModuleCpp.ReverseTwoCallbacksAsyncMethod success => '));

    NativeModules.SampleModuleCpp.ReverseTwoCallbacksAsyncMethod(/*shouldSucceed:*/false,
      getCallback('SampleModuleCpp.ReverseTwoCallbacksAsyncMethod fail => '),
      getCallback('SampleModuleCpp.ReverseTwoCallbacksAsyncMethod success => '));

    var promise1 = NativeModules.SampleModuleCpp.ExplicitPromiseMethod();
    promise1.then(getCallback('SampleModuleCpp.ExplicitPromiseMethod then => ')).catch(getErrorCallback('SampleModuleCpp.ExplicitPromiseMethod catch => '));

    var promise2 = NativeModules.SampleModuleCpp.ExplicitPromiseMethodWithArgs(numberArg);
    promise2.then(getCallback('SampleModuleCpp.ExplicitPromiseMethodWithArgs then => ')).catch(getErrorCallback('SampleModuleCpp.ExplicitPromiseMethodWithArgs catch => '));

    var promise3 = NativeModules.SampleModuleCpp.NegateAsyncPromise(5);
    promise3.then(getCallback('SampleModuleCpp.NegateAsyncPromise then => ')).catch(getErrorCallback('SampleModuleCpp.NegateAsyncPromise catch => '));

    var promise4 = NativeModules.SampleModuleCpp.NegateAsyncPromise(-5);
    promise4.then(getCallback('SampleModuleCpp.NegateAsyncPromise then => ')).catch(getErrorCallback('SampleModuleCpp.NegateAsyncPromise catch => '));

    NativeModules.SampleModuleCpp.callDistanceFunction({x: 2, y: 3}, {x: 5, y: 6});

    NativeModules.SampleModuleCpp.EmitJSEvent1(42);
    NativeModules.SampleModuleCpp.EmitJSEvent2(7, 51);
    NativeModules.SampleModuleCpp.EmitJSEvent3(14, 78);

    //TODO: make sync method accessible only in non-web debugger scenarios
    //log('SampleModuleCpp.SyncReturnMethod => ' + NativeModules.SampleModuleCpp.SyncReturnMethod());
    //log('SampleModuleCpp.SyncReturnMethodWithArgs => ' + NativeModules.SampleModuleCpp.SyncReturnMethodWithArgs(numberArg));
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

  onReloadSampleModuleCS() {
    log('SampleApp.onReloadSampleModuleCS()');
    NativeModules.SampleModuleCS.ReloadInstance();
  }

  onReloadSampleModuleCpp() {
    log('SampleApp.onReloadSampleModuleCpp()');
    NativeModules.SampleModuleCpp.ReloadInstance();
  }

  render() {
    return (
      <View style={styles.container}>

        <MyComp/>
        <Text style={styles.welcome}>
          SampleApp
        </Text>
        <Text style={styles.instructions}>
          This app consumes custom Native Modules and View Managers.
        </Text>

        <Button onPress={() => { MyModule.voidFunc(); }} title="Call MyModule tests"/>

        <Button onPress={() => { this.onPressSampleModuleCS(); }} title="Call SampleModuleCS!" disabled={NativeModules.SampleModuleCS == null} />
        <Button onPress={() => { this.onPressSampleModuleCpp(); }} title="Call SampleModuleCpp!" disabled={NativeModules.SampleModuleCpp == null} />

        <CustomUserControlCS style={styles.customcontrol} label="CustomUserControlCS!" ref={(ref) => { this._CustomUserControlCSRef = ref; }} onLabelChanged={(evt) => { this.onLabelChangedCustomUserControlCS(evt); }} />
        <Button onPress={() => { this.onPressCustomUserControlCS(); }} title="Call CustomUserControlCS Commands!" />

        <CustomUserControlCpp style={styles.customcontrol} label="CustomUserControlCpp!" ref={(ref) => { this._CustomUserControlCppRef = ref; }} onLabelChanged={(evt) => { this.onLabelChangedCustomUserControlCpp(evt); }} />
        <Button onPress={() => { this.onPressCustomUserControlCpp(); }} title="Call CustomUserControlCpp Commands!" />

        <Button onPress={() => { this.onReloadSampleModuleCS(); }} title="Reload from SampleModuleCS" disabled={NativeModules.SampleModuleCS == null} />
        <Button onPress={() => { this.onReloadSampleModuleCpp(); }} title="Reload from SampleModuleCpp" disabled={NativeModules.SampleModuleCpp == null} />

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
