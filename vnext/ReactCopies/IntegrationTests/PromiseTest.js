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

const React = require('react');
const ReactNative = require('react-native');
const {View} = ReactNative;
const {TestModule} = ReactNative.NativeModules;

class PromiseTest extends React.Component<{...}> {
  shouldResolve: boolean = false;
  shouldReject: boolean = false;
  shouldSucceedAsync: boolean = false;
  shouldThrowAsync: boolean = false;

  componentDidMount() {
    Promise.all([
      this.testShouldResolve(),
      this.testShouldReject(),
      this.testShouldSucceedAsync(),
      this.testShouldThrowAsync(),
    ]).then(() =>
      TestModule.markTestPassed(
        this.shouldResolve &&
          this.shouldReject &&
          this.shouldSucceedAsync &&
          this.shouldThrowAsync,
      ),
    );
  }

  testShouldResolve: () => any = () => {
    return TestModule.shouldResolve()
      .then(() => (this.shouldResolve = true))
      .catch(() => (this.shouldResolve = false));
  };

  testShouldReject: () => any = () => {
    return TestModule.shouldReject()
      .then(() => (this.shouldReject = false))
      .catch(() => (this.shouldReject = true));
  };

  testShouldSucceedAsync: () => Promise<any> = async (): Promise<any> => {
    try {
      await TestModule.shouldResolve();
      this.shouldSucceedAsync = true;
    } catch (e) {
      this.shouldSucceedAsync = false;
    }
  };

  testShouldThrowAsync: () => Promise<any> = async (): Promise<any> => {
    try {
      await TestModule.shouldReject();
      this.shouldThrowAsync = false;
    } catch (e) {
      this.shouldThrowAsync = true;
    }
  };

  render(): React.Node {
    return <View />;
  }
}

PromiseTest.displayName = 'PromiseTest';

module.exports = PromiseTest;
