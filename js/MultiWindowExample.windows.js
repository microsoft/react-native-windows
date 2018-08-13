/**
 * Copyright (c) Microsoft Corporation and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @flow
 */
'use strict';

var React = require('react');
var ReactNative = require('react-native');
var {
  AppRegistry,
  Text,
  TouchableHighlight,
  View,
  StyleSheet,
  NativeModules
} = ReactNative;

class NewWindowExampleBlock extends React.Component {
  render() {
    return (
      <View>
        <TouchableHighlight style={styles.wrapper}
          onPress={() => NativeModules.CrossWindow.newWindow()}>
          <View style={styles.button}>
            <Text>Creates a new top level window (a clone of RNTester main window)</Text>
          </View>
        </TouchableHighlight>
      </View>
    );
  }
}

class SecondaryWindow extends React.Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          This is a test secondary window!
        </Text>
        <Text style={styles.instructions}>
          Please don't close it and let the test finish!
        </Text>
      </View>
    );
  }
}

class StressOpenCloseExampleBlock extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      status: 'Not running.'
    };
  }

  render() {
    return (
      <View>
        <TouchableHighlight style={styles.wrapper}
          onPress={() => this.runStress()}>
          <View style={styles.button}>
            <Text>Start a sequence of rapidly opening/closing of some test windows</Text>
          </View>
        </TouchableHighlight>
        <Text style={styles.status}> {this.state.status}</Text>
      </View>
    );
  }

  runStress() {
    this.setState({status: 'Running...'});
    NativeModules.CrossWindow.runStressOpenCloseTest('SecondaryWindow').then(() => this.setState({status: 'Not running.'}));
  }
}

exports.title = 'Multi Window';
exports.description = 'Multi window related tests/examples';
exports.examples = [
  {
    title: 'New top level window',
    render: function() {
      return (
        <NewWindowExampleBlock />
      );
    }
  },
  {
    title: 'Stress open/close',
    render: function() {
      return (
        <StressOpenCloseExampleBlock />
      );
    }
  },
];

var styles = StyleSheet.create({
  wrapper: {
    borderRadius: 5,
    marginBottom: 5,
  },
  button: {
    backgroundColor: '#eeeeee',
    padding: 10,
  },
  status: {
    padding: 10,
  },
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
  }
});

AppRegistry.registerComponent('SecondaryWindow', () => SecondaryWindow);
