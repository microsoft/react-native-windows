/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {Image} = ReactNative;
const {TestModule} = ReactNative.NativeModules;

class ImageSnapshotTest extends React.Component<{...}> {
  componentDidMount(): void {
    if (!TestModule.verifySnapshot) {
      throw new Error('TestModule.verifySnapshot not defined.');
    }
  }

  done: (success: boolean) => void = (success: boolean) => {
    TestModule.markTestPassed(success);
  };

  render(): React.Node {
    return (
      <Image
        source={require('./blue_square.png')}
        defaultSource={require('./red_square.png')}
        onLoad={() => TestModule.verifySnapshot(this.done)}
      />
    );
  }
}

ImageSnapshotTest.displayName = 'ImageSnapshotTest';

module.exports = ImageSnapshotTest;
