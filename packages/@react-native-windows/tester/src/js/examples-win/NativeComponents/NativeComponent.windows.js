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

import React, {useState} from 'react';
import {Text, View} from 'react-native';
import CustomXamlComponentWithNativeLayout from './CustomXamlComponentWithNativeLayoutNativeComponent';

const NativeComponentWithNativeLayout = () => {
  let [log, setLog] = useState('');
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
      <Text style={{color: 'red'}}>{log}</Text>
      <CustomXamlComponentWithNativeLayout
        label="This is a Xaml Button set to ellipisify on truncation"
        style={{flexShrink: 1}}
        onMyEvent={arg => {
          setLog(
            log +
              '\nRecieved MyEvent: ' +
              JSON.stringify(arg.nativeEvent) +
              '\n',
          );
        }}
      />
      <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
      <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
      <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
      <View style={{width: 100, height: 100, backgroundColor: 'pink'}} />
      <View style={{width: 100, height: 100, backgroundColor: 'gray'}} />
    </View>
  );
};

exports.displayName = 'NativeFabricComponent';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Fabric Native Component';
//exports.documentationURL = 'https://reactnative.dev/docs/button';
exports.description =
  'Sample Fabric Native Component that sizes based on max desired size of native XAML contained within';

exports.examples = [
  {
    title: 'Native Component',
    render: function (): React.Node {
      return <NativeComponentWithNativeLayout />;
    },
  },
];
