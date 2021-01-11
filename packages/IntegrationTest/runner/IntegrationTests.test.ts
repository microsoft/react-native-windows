/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';

import {block, skip} from './lib/TestDescription';
import {registerTests} from './lib/BaseTest';
import discoverTests from './lib/discoverTests';

registerTests([
  // Add our own auto-registered tests
  ...discoverTests(path.join(__dirname, '..', 'tests')),

  block('BuiltinTests', [
    // Add working upstream tests
    'IntegrationTestHarnessTest',
    'AppEventsTest',
    'PromiseTest',
    'WebSocketTest',

    // This one is seemingly broken upstream since
    // https://github.com/facebook/react-native/commit/61346d303a44b643b6029835018518d48c285a69
    skip('TimersTest'),

    // Our builtin implementation doens't implement mergeItems
    skip('AsyncStorageTest'),

    // ref.measure returns bad results for us (#5934)
    skip('LayoutEventsTest'),

    // Skip SyncMethodTest because we don't implement RNTesterTestModule and want
    // to allow web debugging
    skip('SyncMethodTest'),

    // globalEvalWithSourceUrl is only set when using JSIExecutor in debug. I.e. it
    // will not work when web debugging
    skip('GlobalEvalWithSourceUrlTest'),

    // Don't bother marking SimpleSnapshotTest, ImageSnapshotTest,
    // ImageCachePolicyTest, or AccessibilityManagerTest as skipped since they're
    // iOS specific and won't be enabled
  ]),
]);
