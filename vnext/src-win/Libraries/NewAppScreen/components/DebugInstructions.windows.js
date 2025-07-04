/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import StyleSheet from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text';
import Platform from '../../Utilities/Platform';
import * as React from 'react';

const styles = StyleSheet.create({
  highlight: {
    fontWeight: '700',
  },
});

const DebugInstructions = (): React.Node => (
  <Text>
    Press <Text style={styles.highlight}>Ctrl+Shift+D</Text> to open the Dev
    Menu.
  </Text>
);

export default DebugInstructions;
