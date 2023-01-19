/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

'use strict';
import type {DatePickerIOSType} from './DatePickerIOS.flow';

import StyleSheet from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text';
import View from '../View/View';
import * as React from 'react';

class DummyDatePickerIOS extends React.Component {
  render() {
    return (
      <View style={[styles.dummyDatePickerIOS, this.props.style]}>
        <Text style={styles.datePickerText}>
          DatePickerIOS is not supported on this platform!
        </Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  dummyDatePickerIOS: {
    height: 100,
    width: 300,
    backgroundColor: '#ffbcbc',
    borderWidth: 1,
    borderColor: 'red',
    alignItems: 'center',
    justifyContent: 'center',
    margin: 10,
  },
  datePickerText: {
    color: '#333333',
    margin: 20,
  },
});

module.exports = (DummyDatePickerIOS: DatePickerIOSType);
