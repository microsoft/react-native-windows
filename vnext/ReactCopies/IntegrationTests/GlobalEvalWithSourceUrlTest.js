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

import type {ExtendedError} from 'react-native/Libraries/Core/ExtendedError';

const React = require('react');
const ReactNative = require('react-native');
const parseErrorStack = require('react-native/Libraries/Core/Devtools/parseErrorStack');
const {View} = ReactNative;

const {TestModule} = ReactNative.NativeModules;

class GlobalEvalWithSourceUrlTest extends React.Component<{...}> {
  componentDidMount(): void {
    if (typeof global.globalEvalWithSourceUrl !== 'function') {
      throw new Error(
        'Expected to find globalEvalWithSourceUrl function on global object but found ' +
          typeof global.globalEvalWithSourceUrl,
      );
    }
    const value = global.globalEvalWithSourceUrl('42');
    if (value !== 42) {
      throw new Error(
        'Expected globalEvalWithSourceUrl(expression) to return a value',
      );
    }
    let syntaxError: ?ExtendedError;
    try {
      global.globalEvalWithSourceUrl('{');
    } catch (e) {
      syntaxError = e;
    }
    if (!syntaxError) {
      throw new Error(
        'Expected globalEvalWithSourceUrl to throw on a syntax error',
      );
    }
    // Hermes throws an Error instead of a SyntaxError
    // https://github.com/facebook/hermes/issues/400
    if (
      syntaxError.jsEngine !== 'hermes' &&
      !(syntaxError instanceof SyntaxError)
    ) {
      throw new Error(
        'Expected globalEvalWithSourceUrl to throw SyntaxError on a syntax error',
      );
    }
    const url = 'http://example.com/foo.js';
    let error;
    try {
      global.globalEvalWithSourceUrl('throw new Error()', url);
    } catch (e) {
      error = e;
    }
    if (!error) {
      throw new Error(
        'Expected globalEvalWithSourceUrl to throw an Error object',
      );
    }
    const parsedStack = parseErrorStack(error?.stack);
    if (parsedStack[0].file !== url) {
      throw new Error(
        `Expected first eval stack frame to be in ${url} but found ${String(
          parsedStack[0].file,
        )}`,
      );
    }
    TestModule.markTestCompleted();
  }

  render(): React.Node {
    return <View />;
  }
}

GlobalEvalWithSourceUrlTest.displayName = 'GlobalEvalWithSourceUrlTest';

module.exports = GlobalEvalWithSourceUrlTest;
