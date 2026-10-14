'use strict';

import React from 'react';
import {View} from 'react-native';
import {CustomTextInput} from '@rnw-scripts/sample-custom-component';
import RNTesterText from '../../components/RNTesterText';

const CustomTextInputExample = () => {
  return (
    <View testID="custom-textinput-root-1" accessible accessibilityLabel='example root'>
      <RNTesterText>The below text input is a custom text input component</RNTesterText>
      <CustomTextInput style={{width: 500, borderWidth: 1}} accessible accessibilityLabel='Custom TextInput' testID="custom-textinput-1"/>
    </View>
  );
}

exports.displayName = 'CustomTextInputExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Custom TextInput component Example';
exports.description =
  'Sample of creating a custom native TextInput component';

exports.examples = [
  {
    title: 'Custom Native Accessibility',
    render: function (): React.Node {
      return (
        <CustomTextInputExample />
      );
    },
  }
];
