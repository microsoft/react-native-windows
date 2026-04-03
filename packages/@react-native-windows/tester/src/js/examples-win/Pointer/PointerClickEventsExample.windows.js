/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {RNTesterModuleExample} from '../../types/RNTesterTypes';

const React = require('react');
import {Pressable, StyleSheet, View} from 'react-native';
import RNTesterText from '../../components/RNTesterText';

function PointerClickEventsExample(): React.Node {
  const [text, setText] = React.useState(
    'onClick should cause the box to go red, onAuxClick will cause it go green',
  );
  const [bgColor, setBgColor]= React.useState('gray');
  return (
    <View>
      <View
        accessible
        testID="pointer-click-target"
        style={[styles.targetBox, {backgroundColor: bgColor}]}
        onClick={(e) => {
          setBgColor('red');
          setText('onClick.nativeEvent: ' + JSON.stringify(e.nativeEvent, null, 2));
        }}
        onAuxClick={(e) => {
          setBgColor('green');
          setText('onAuxClick.nativeEvent: ' + JSON.stringify(e.nativeEvent, null, 2));
        }}
      />
     <RNTesterText accessible testID='pointer-click-text'>{text}</RNTesterText>
    </View>

  );
}

exports.displayName = 'PointerClickExample';
exports.framework = 'React';
exports.category = 'Basic';
exports.title = 'Pointer Clicks';
exports.documentationURL =
  'https://developer.mozilla.org/en-US/docs/Web/API/Element/auxclick_event';
exports.description =
  'Tests that onClick and onAuxClick work.';

exports.examples = [
  {
    title: 'onClick/onAuxClick test',
    description:
      'Click the box with different pointers/buttons to test pointer click events.',
    render: function (): React.Node {
      return <PointerClickEventsExample />;
    },
  },
] as Array<RNTesterModuleExample>;

const styles = StyleSheet.create({
  targetBox: {
    width: 200,
    height: 200,
    margin: 10,
    borderRadius: 10,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
