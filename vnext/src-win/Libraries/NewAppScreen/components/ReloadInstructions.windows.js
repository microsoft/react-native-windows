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
<<<<<<< Upstream
import Platform from '../../Utilities/Platform';
import * as React from 'react';
=======
import React from 'react';
>>>>>>> Override

const styles = StyleSheet.create({
  highlight: {
    fontWeight: '700',
  },
});

const ReloadInstructions = (): React.Node => (
  <Text>
    Press <Text style={styles.highlight}>Ctrl+Shift+D</Text> and select{' '}
    <Text style={styles.highlight}>Reload javascript</Text> to reload your app's
    code.
  </Text>
);

export default ReloadInstructions;
