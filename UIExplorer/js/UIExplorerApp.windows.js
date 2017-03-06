/**
 * Copyright (c) 2013-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * The examples provided by Facebook are for non-commercial testing and
 * evaluation purposes only.
 *
 * Facebook reserves all rights not expressly granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL
 * FACEBOOK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @providesModule UIExplorerApp
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
const UIExplorerActions = require('./UIExplorerActions');
const UIExplorerExampleContainer = require('./UIExplorerExampleContainer');
const UIExplorerExampleList = require('./UIExplorerExampleList');
const UIExplorerList = require('./UIExplorerList');
const UIExplorerNavigationReducer = require('./UIExplorerNavigationReducer');
const UIExplorerHeaderWindows = require('./UIExplorerHeaderWindows');
const UIManager = require('UIManager');
const URIActionMap = require('./URIActionMap');
const View = require('View');

import type { UIExplorerNavigationState } from './UIExplorerNavigationReducer';

const DRAWER_WIDTH_LEFT = 56;

type Props = {
  exampleFromAppetizeParams: string,
};

const APP_STATE_KEY = 'UIExplorerAppState.v2';

class UIExplorerApp extends React.Component {
  props: Props;
  state: UIExplorerNavigationState;

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
          this.setState(UIExplorerNavigationReducer(null, initialAction));
          return;
        }
        const storedState = JSON.parse(storedString);
        if (launchAction) {
          this.setState(UIExplorerNavigationReducer(storedState, launchAction));
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
        <UIExplorerExampleList
          list={UIExplorerList}
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
      const ExampleModule = UIExplorerList.Modules[openExample];
      if (ExampleModule.external) {
        return (
          <ExampleModule
            onExampleExit={() => {
              this._handleAction(UIExplorerActions.Back());
            }}
            ref={(example) => { this._exampleRef = example; }}
          />
        );
      } else if (ExampleModule) {
        return (
          <View style={styles.container}>
            <UIExplorerHeaderWindows
              onPress={() => this.splitView.openPane()} 
              title={ExampleModule.title}
              style={styles.header}
            />
            <UIExplorerExampleContainer
              module={ExampleModule}
              ref={(example) => { this._exampleRef = example; }}
            />
          </View>
        );
      }
    }

    return (
      <View style={styles.container}>
        <UIExplorerHeaderWindows
          onPress={() => this.splitView.openPane()} 
          title="UIExplorer"
          style={styles.header}
        />
        <UIExplorerExampleList
          onNavigate={this._handleAction}
          list={UIExplorerList}
        />
      </View>
    );
  }

  _handleAction = (action: Object): boolean => {
    this.splitView && this.splitView.closePane();
    const newState = UIExplorerNavigationReducer(this.state, action);
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
    return this._handleAction(UIExplorerActions.Back());
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

AppRegistry.registerComponent('UIExplorerApp', () => UIExplorerApp);

module.exports = UIExplorerApp;
