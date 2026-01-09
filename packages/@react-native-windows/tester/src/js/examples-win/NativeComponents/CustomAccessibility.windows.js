'use strict';

import React from 'react';
import {View} from 'react-native';
import {CustomAccessibility} from 'sample-custom-component';
import RNTesterText from '../../components/RNTesterText';

const CustomAccessibilityExample = () => {
  return (
    <View testID="custom-accessibility-root-1">
      <RNTesterText>The below view should have custom accessibility</RNTesterText>
      <CustomAccessibility style={{width: 500, height: 500, backgroundColor:'green'}} accessible accessibilityLabel='accessibility should not show this, as native overrides it' testID="custom-accessibility-1"/>
    </View>
  );
}

exports.displayName = 'CustomAccessibilityExample';
exports.framework = 'React';
exports.category = 'UI';
exports.title = 'Custom Native Accessibility Example';
exports.description =
  'Sample of a Custom Native Component overriding default accessibility';

exports.examples = [
  {
    title: 'Custom Native Accessibility',
    render: function (): React.Node {
      return (
        <CustomAccessibilityExample />
      );
    },
  }
];
