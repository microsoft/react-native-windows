/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {assert} from 'chai';
import {NativeModules, NativeEventEmitter} from 'react-native';
import {functionTest} from './lib/TestDefinition';

const testModuleCs = NativeModules.TestModuleCS;

interface Point {
  x: number;
  y: number;
}

/**
 * Test for Constants
 */

functionTest('NumberConstant', () => {
  const pi = testModuleCs.NumberConstant;
  assert.isTrue(pi > 3.14 && pi < 3.15);
});

functionTest('StringConstant', () => {
  const msg = testModuleCs.StringConstant;
  assert.equal(msg, 'Hello World');
});

functionTest('PointConstant', () => {
  const point = testModuleCs.PointConstant;
  assert.equal(point.x, 10);
  assert.equal(point.y, 20);
  assert.equal(point.z, undefined);
});

functionTest('ArrayConstant', () => {
  const arr = testModuleCs.ArrayConstant;
  assert.equal(arr.length, 3);
  assert.equal(arr[0], 'one');
  assert.equal(arr[1], 'two');
  assert.equal(arr[2], 'three');
});

functionTest('NullConstant', () => {
  const n = testModuleCs.NullConstant;
  assert.equal(n, null);
});

/**
 * Tests for Methods
 */

functionTest(
  'GetAndSetValue',
  () =>
    new Promise((resolve, _) => {
      testModuleCs.SetValue(123);
      testModuleCs.GetValue((result: number) => {
        assert.equal(result, 123);
        resolve();
      });
    }),
);

functionTest('GetAndSetValueAsync', async () => {
  await testModuleCs.SetValueAsync(123);
  const result = await testModuleCs.GetValueAsync();
  assert.equal(result, 123);
});

functionTest(
  'Add',
  () =>
    new Promise((resolve, _) => {
      testModuleCs.Add(10, 20, (result: number) => {
        assert.equal(result, 30);
        resolve();
      });
    }),
);

functionTest('AddAsync', async () => {
  const result = await testModuleCs.AddAsync(10, 20);
  assert.equal(result, 30);
});

functionTest(
  'Concat',
  () =>
    new Promise((resolve, _) => {
      testModuleCs.Concat('abc', 20, (result: string) => {
        assert.equal(result, 'abc20');
        resolve();
      });
    }),
);

functionTest('ConcatAsync', async () => {
  const result = await testModuleCs.ConcatAsync('abc', 20);
  assert.equal(result, 'abc20');
});

functionTest(
  'DeltaPoint',
  () =>
    new Promise((resolve, _) => {
      testModuleCs.DeltaPoint(
        {x: 1, y: 10},
        {x: 10, y: 100},
        (result: Point) => {
          assert.equal(result.x, 9);
          assert.equal(result.y, 90);
          resolve();
        },
      );
    }),
);

functionTest('DeltaPointAsync', async () => {
  const result = await testModuleCs.DeltaPointAsync(
    {x: 10, y: 100},
    {x: 1, y: 10},
  );

  assert.equal(result.x, 9);
  assert.equal(result.y, 90);
});

functionTest(
  'Failure',
  () =>
    new Promise((resolve, reject) => {
      testModuleCs.Failure(
        () => {
          reject(new Error('This call should have failed'));
        },
        (e: Error) => {
          assert.isTrue(e.message.includes('ExpectedFailure'));
          resolve();
        },
      );
    }),
);

functionTest('FailureAsync', async () => {
  let seenFailure = false;
  try {
    await testModuleCs.FailureAsync();
  } catch (e) {
    assert.isTrue(e.message.includes('ExpectedFailure'));
    seenFailure = true;
  }

  assert.isTrue(seenFailure, 'Expected to encounter a failure');
});

/**
 * Test for Events
 */

functionTest(
  'TestEvent',
  () =>
    new Promise((resolve, _) => {
      const eventEmitter = new NativeEventEmitter(testModuleCs);

      eventEmitter.addListener('TestEvent', x => {
        assert.equal(x, 123);
        resolve();
      });

      testModuleCs.RaiseTestEvent(123);
    }),
);
