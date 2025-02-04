/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

import React from 'react';
import {View} from 'react-native';

import {DrawingIsland} from 'sample-custom-component';

const DrawingIslandExample = () => {
  return (
    <View
      style={{
        borderRadius: 0,
        margin: 10,
        borderWidth: 2,
        flexWrap: 'wrap',
        flexDirection: 'row',
        gap: 5,
      }}>
      <View style={{width: 200, height: 200, backgroundColor: 'green'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'red'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'blue'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'pink'}} />
      <DrawingIsland style={{flex: 1, minWidth: 200}} />
      <View style={{width: 200, height: 200, backgroundColor: 'green'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'red'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'blue'}} />
      <View style={{width: 200, height: 200, backgroundColor: 'pink'}} />
    </View>
  );
};

exports.displayName = 'DrawingIslandExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Drawing Island Example';
exports.description =
  'Sample Fabric Native Component that contains a custom ContentIsland';

exports.examples = [
  {
    title: 'Drawing Island',
    render: function (): React.Node {
      return <DrawingIslandExample />;
    },
  },
];
