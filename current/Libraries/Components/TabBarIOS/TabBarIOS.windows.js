/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule TabBarIOS
 * @flow
 */

'use strict';

const React = require('React');
const StyleSheet = require('StyleSheet');
const TabBarItemIOS = require('TabBarItemIOS');
const View = require('View');

class DummyTabBarIOS extends React.Component<$FlowFixMeProps> {
  static Item = TabBarItemIOS;

  render() {
    return (
      <View style={[this.props.style, styles.tabGroup]}>
        {this.props.children}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  tabGroup: {
    flex: 1,
  }
});

module.exports = DummyTabBarIOS;
