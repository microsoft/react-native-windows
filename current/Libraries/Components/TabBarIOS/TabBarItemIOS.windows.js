/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule TabBarItemIOS
 */

'use strict';

var React = require('React');
var View = require('View');
var StyleSheet = require('StyleSheet');

class DummyTab extends React.Component {
  render() {
    if (!this.props.selected) {
      return <View />;
    }
    return (
      <View style={[this.props.style, styles.tab]}>
        {this.props.children}
      </View>
    );
  }
}

var styles = StyleSheet.create({
  tab: {
    // TODO(5405356): Implement overflow: visible so position: absolute isn't useless
    // position: 'absolute',
    top: 0,
    right: 0,
    bottom: 0,
    left: 0,
    borderColor: 'red',
    borderWidth: 1,
  }
});

module.exports = DummyTab;
