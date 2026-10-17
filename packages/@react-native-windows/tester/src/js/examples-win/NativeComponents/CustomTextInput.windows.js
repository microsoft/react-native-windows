/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

import React from 'react';
import {View} from 'react-native';
import {CustomTextInput} from '@rnw-scripts/sample-custom-component';
import RNTesterText from '../../components/RNTesterText';

const CustomTextInputExample = () => {
  const [text, setText] = React.useState('hello');
  const [lastChange, setLastChange] = React.useState('');
  const [lastDefaultChange, setLastDefaultChange] = React.useState('');
  return (
    <View
      testID="custom-textinput-root-1"
      accessible
      accessibilityLabel="example root">
      <RNTesterText>
        The below text input is a custom text input component
      </RNTesterText>
      <CustomTextInput
        style={{width: 500, borderWidth: 1}}
        accessible
        accessibilityLabel="Custom TextInput"
        testID="custom-textinput-1"
        value={text}
        onChange={event => setLastChange(event.nativeEvent.text)}
        onChangeText={value => setText(value.toUpperCase())}
      />
      <RNTesterText testID="custom-textinput-change">{lastChange}</RNTesterText>
      <CustomTextInput
        style={{width: 500, borderWidth: 1}}
        accessible
        testID="custom-textinput-default"
        defaultValue="default value"
        onChangeText={setLastDefaultChange}
      />
      <RNTesterText testID="custom-textinput-default-change">
        {lastDefaultChange}
      </RNTesterText>
      <CustomTextInput
        style={{width: 500, borderWidth: 1}}
        accessible
        testID="custom-textinput-fixed"
        value="fixed"
      />
    </View>
  );
};

exports.displayName = 'CustomTextInputExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Custom TextInput component Example';
exports.description = 'Sample of creating a custom native TextInput component';

exports.examples = [
  {
    title: 'Custom Native Accessibility',
    render: function (): React.Node {
      return <CustomTextInputExample />;
    },
  },
];
