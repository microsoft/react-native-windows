/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @providesModule SegmentedControlIOS
 */

'use strict';

var React = require('React');
var StyleSheet = require('StyleSheet');
var Text = require('Text');
var View = require('View');

class DummySegmentedControlIOS extends React.Component {
  render() {
    return (
      <View style={[styles.dummy, this.props.style]}>
        <Text style={styles.text}>
          SegmentedControlIOS is not supported on this platform!
        </Text>
      </View>
    );
  }
}

var styles = StyleSheet.create({
  dummy: {
    width: 120,
    height: 50,
    backgroundColor: '#ffbcbc',
    borderWidth: 1,
    borderColor: 'red',
    alignItems: 'center',
    justifyContent: 'center',
  },
  text: {
    color: '#333333',
    margin: 5,
    fontSize: 10,
  }
});

module.exports = DummySegmentedControlIOS;
