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
 * @providesModule RNTesterApp
 * @flow
 */
'use strict';

const AppRegistry = require('AppRegistry');
const AsyncStorage = require('AsyncStorage');
const BackAndroid = require('BackAndroid');
const Dimensions = require('Dimensions');
const SplitViewWindows = require('SplitViewWindows');
const Linking = require('Linking');
const React = require('react');
const StatusBar = require('StatusBar');
const StyleSheet = require('StyleSheet');
const ToolbarAndroid = require('ToolbarAndroid');
const RNTesterActions = require('./RNTesterActions');
const RNTesterExampleContainer = require('./RNTesterExampleContainer');
const RNTesterExampleList = require('./RNTesterExampleList');
const RNTesterList = require('./RNTesterList');
const RNTesterNavigationReducer = require('./RNTesterNavigationReducer');
const RNTesterHeaderWindows = require('./RNTesterHeaderWindows');
const UIManager = require('UIManager');
const URIActionMap = require('./URIActionMap');
const View = require('View');

import type { RNTesterNavigationState } from './RNTesterNavigationReducer';

const DRAWER_WIDTH_LEFT = 56;

type Props = {
  exampleFromAppetizeParams: string,
};

const APP_STATE_KEY = 'RNTesterAppState.v2';

class RNTesterApp extends React.Component {
  props: Props;
  state: RNTesterNavigationState;

  componentWillMount() {
    BackAndroid.addEventListener('hardwareBackPress', this._handleBackButtonPress.bind(this));
  }

  componentDidMount() {
    Linking.getInitialURL().then((url) => {
      AsyncStorage.getItem(APP_STATE_KEY, (err, storedString) => {
        const exampleAction = URIActionMap(this.props.exampleFromAppetizeParams);
        const urlAction = URIActionMap(url);
        const launchAction = exampleAction || urlAction;
        if (err || !storedString) {
          const initialAction = launchAction || {type: 'InitialAction'};
          this.setState(RNTesterNavigationReducer(null, initialAction));
          return;
        }
        const storedState = JSON.parse(storedString);
        if (launchAction) {
          this.setState(RNTesterNavigationReducer(storedState, launchAction));
          return;
        }
        this.setState(storedState);
      });
    });
  }

  render() {
    if (!this.state) {
      return null;
    }
    return (
      <SplitViewWindows
        panePosition={SplitViewWindows.positions.Left}
        paneWidth={Dimensions.get('window').width - DRAWER_WIDTH_LEFT}
        keyboardDismissMode="on-drag"
        onPaneOpen={() => {
          this._overrideBackPressForPane = true;
        }}
        onPaneClose={() => {
          this._overrideBackPressForPane = false;
        }}
        ref={(splitView) => { this.splitView = splitView; }}
        renderPaneView={this._renderPaneContent}
        statusBarBackgroundColor="#589c90">
        {this._renderApp()}
      </SplitViewWindows>
    );
  }

  _renderPaneContent = () => {
    return (
      <View style={styles.paneContentWrapper}>
        <RNTesterExampleList
          list={RNTesterList}
          displayTitleRow={true}
          disableSearch={true}
          onNavigate={this._handleAction}
        />
      </View>
    );
  };

  _renderApp() {
    const {
      openExample,
    } = this.state;

    if (openExample) {
      const ExampleModule = RNTesterList.Modules[openExample];
      if (ExampleModule.external) {
        return (
          <ExampleModule
            onExampleExit={() => {
              this._handleAction(RNTesterActions.Back());
            }}
            ref={(example) => { this._exampleRef = example; }}
          />
        );
      } else if (ExampleModule) {
        return (
          <View style={styles.container}>
            <RNTesterHeaderWindows
              onPress={() => this.splitView.openPane()} 
              title={ExampleModule.title}
              style={styles.header}
            />
            <RNTesterExampleContainer
              module={ExampleModule}
              ref={(example) => { this._exampleRef = example; }}
            />
          </View>
        );
      }
    }

    return (
      <View style={styles.container}>
        <RNTesterHeaderWindows
          onPress={() => this.splitView.openPane()} 
          title="RNTester"
          style={styles.header}
        />
        <RNTesterExampleList
          onNavigate={this._handleAction}
          list={RNTesterList}
        />
      </View>
    );
  }

  _handleAction = (action: Object): boolean => {
    this.splitView && this.splitView.closePane();
    const newState = RNTesterNavigationReducer(this.state, action);
    if (this.state !== newState) {
      this.setState(
        newState,
        () => AsyncStorage.setItem(APP_STATE_KEY, JSON.stringify(this.state))
      );
      return true;
    }
    return false;
  };

  _handleBackButtonPress() {
    if (this._overrideBackPressForPane) {
      // This hack is necessary because split view provides an imperative API
      // with open and close methods. This code would be cleaner if the split
      // view provided an `isOpen` prop and allowed us to pass a `onPaneClose` handler.
      this.splitView && this.splitView.closePane();
      return true;
    }
    if (
      this._exampleRef &&
      this._exampleRef.handleBackAction &&
      this._exampleRef.handleBackAction()
    ) {
      return true;
    }
    return this._handleAction(RNTesterActions.Back());
  };
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  toolbar: {
    backgroundColor: '#E9EAED',
    height: 56,
  },
  paneContentWrapper: {
    flex: 1,
    paddingTop: StatusBar.currentHeight,
    backgroundColor: 'white',
  },
});

AppRegistry.registerComponent('RNTesterApp', () => RNTesterApp);

module.exports = RNTesterApp;
