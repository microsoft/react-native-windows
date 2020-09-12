/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import {useEffect} from 'react';
import {AppRegistry, NativeModules, View} from 'react-native';

const TestModule: {markTestPassed: (passed: boolean) => void} =
  NativeModules.TestModule;

type TestFunction = (() => void) | (() => Promise<void>);
type TestComponent = React.ComponentType<{
  pass: () => void;
  fail: (err: Error) => void;
}>;

/**
 * Define a new synchronus or asynchronus test function. The test may be failed
 * by throwing
 */
export const functionTest = Object.assign(
  (name: string, fn: TestFunction) => {
    const BoundTest: React.FunctionComponent = () => {
      useEffect(() => {
        (async () => {
          try {
            const testResult = fn();
            if (testResult instanceof Promise) {
              await testResult;
            }
          } catch (ex) {
            // Need to force a redbox instead of YellowBox for unhandled
            // promise rejection
            ErrorUtils.getGlobalHandler()(ex);
          }

          TestModule.markTestPassed(true);
        })();
      });

      return <View />;
    };

    AppRegistry.registerComponent(name, () => BoundTest);
  },
  {skip: (_name: string, _fn: TestFunction) => {}},
);

/**
 * Define a new test component which is responsible for passing/failing a test
 */
export const componentTest = Object.assign(
  (name: string, Component: TestComponent) => {
    const BoundComponent: React.FunctionComponent = () => {
      return (
        <Component
          pass={() => TestModule.markTestPassed(true)}
          fail={err => {
            throw err;
          }}
        />
      );
    };

    AppRegistry.registerComponent(name, () => BoundComponent);
  },
  {skip: (_name: string, _Component: TestComponent) => {}},
);
