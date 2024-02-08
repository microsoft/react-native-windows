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
import {Text, View} from 'react-native';
import {CustomXamlComponentWithYogaLayout} from './CustomXamlComponentWithYogaLayoutNativeComponent';

exports.displayName = 'NativeFabricComponentYoga';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Fabric Native Component Yoga';
//exports.documentationURL = 'https://reactnative.dev/docs/button';
exports.description =
  'Sample Fabric Native Component that places native XAML inside a container sized by yoga';

exports.examples = [
  {
    title: 'Native Component',
    render: function (): React.Node {
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
          <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'pink'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'gray'}} />
          <Text style={{color: 'gray'}}>This is RN Text</Text>
          <CustomXamlComponentWithYogaLayout
            label="This is a Xaml Button set to ellipisify on truncation"
            style={{flex: 1, minWidth: 100}}
          />
          <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'pink'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'gray'}} />
        </View>
      );
    },
  },
];
