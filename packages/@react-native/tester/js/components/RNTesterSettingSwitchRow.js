/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

const RNTesterStatePersister = require('../utils/RNTesterStatePersister');
const React = require('react');

const {StyleSheet, Switch, Text, View} = require('react-native');

class RNTesterSettingSwitchRow extends React.Component<
  $FlowFixMeProps,
  $FlowFixMeState,
> {
  UNSAFE_componentWillReceiveProps(newProps: $FlowFixMeProps) {
    const {onEnable, onDisable, persister} = this.props;
    if (newProps.persister.state !== persister.state) {
      newProps.persister.state ? onEnable() : onDisable();
    }
  }
  render(): React.Node {
    const {label, persister} = this.props;
    return (
      <View style={styles.row}>
        <Text>{label}</Text>
        <Switch
          value={persister.state}
          onValueChange={value => {
            persister.setState(() => value);
          }}
        />
      </View>
    );
  }
}
const styles = StyleSheet.create({
  row: {
    padding: 10,
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
});
/* $FlowFixMe[cannot-reassign-export] (>=0.85.0 site=react_native_fb) This
 * comment suppresses an error found when Flow v0.85 was deployed. To see the
 * error, delete this comment and run Flow. */
RNTesterSettingSwitchRow = RNTesterStatePersister.createContainer(
  RNTesterSettingSwitchRow,
  {
    cacheKeySuffix: ({label}) => 'Switch:' + label,
    getInitialState: ({initialValue}) => initialValue,
  },
);
module.exports = RNTesterSettingSwitchRow;
