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

export type TestFunction = (() => void) | (() => Promise<void>);

/**
 * Define a new synchronus or asynchronus test function. The test may be failed
 * by throwing
 */
export const functionTest = Object.assign(
  (name: string, fn: TestFunction) => {
    if (AppRegistry.getRunnable(name)) {
      console.error(`Multiple components named "${name}" were registered`);
      const BoundTest = createFailFastTestComponent(
        `Multiple components named "${name}" were registered`,
      );
      AppRegistry.registerComponent(name, () => BoundTest);
    } else {
      const BoundTest = createImperativeTestComponent(fn);
      AppRegistry.registerComponent(name, () => BoundTest);
    }
  },
  {skip: (_name: string, _fn: TestFunction) => {}},
);

export type TestComponentType = React.ComponentType<{
  pass: () => void;
  fail: (err: string) => never;
}>;

/**
 * Define a new test component which is responsible for passing/failing a test
 */
export const componentTest = Object.assign(
  (name: string, Component: TestComponentType) => {
    if (AppRegistry.getRunnable(name)) {
      console.error(`Multiple components named "${name}" were registered`);
      const BoundTest = createFailFastTestComponent(
        `Multiple components named "${name}" were registered`,
      );
      AppRegistry.registerComponent(name, () => BoundTest);
    } else {
      const BoundTest = wrapTestComponent(Component);
      AppRegistry.registerComponent(name, () => BoundTest);
    }
  },
  {skip: (_name: string, _Component: TestComponentType) => {}},
);

/**
 * Create a root component which runs the passed test function
 */
function createImperativeTestComponent(fn: TestFunction): React.FC {
  return () => {
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
          return;
        }

        TestModule.markTestPassed(true);
      })();
    });

    return <View />;
  };
}

/**
 * Wrap a component with test callback properties into a root component
 */
function wrapTestComponent(Component: TestComponentType): React.FC {
  return () => {
    return (
      <Component
        pass={() => TestModule.markTestPassed(true)}
        fail={err => {
          throw new Error(err);
        }}
      />
    );
  };
}

/**
 * Create a test component that immediately fails with an error message
 */
function createFailFastTestComponent(errorMessage: string): React.FC {
  return () => {
    throw new Error(errorMessage);
  };
}
