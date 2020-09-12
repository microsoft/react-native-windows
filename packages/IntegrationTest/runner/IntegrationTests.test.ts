/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';

import {skip} from './lib/SkipTest';
import {registerTestComponents} from './lib/BaseTest';
import discoverTests from './lib/discoverTests';

registerTestComponents([
  // Add working upstream tests
  'IntegrationTestHarnessTest',
  'AppEventsTest',
  'PromiseTest',
  'WebSocketTest',

  // Add our own auto-registered tests
  ...discoverTests(path.join(__dirname, '..', 'tests')),

  // This one is seemingly broken upstream since
  // https://github.com/facebook/react-native/commit/61346d303a44b643b6029835018518d48c285a69
  skip('TimersTest'),

  // Our builtin implementation doens't implement mergeItems
  skip('AsyncStorageTest'),

  // ref.measure returns bad results for us (#5934)
  skip('LayoutEventsTest'),

  // Native snapshot checks not implemented
  skip('SimpleSnapshotTest'),
  skip('ImageSnapshotTest'),

  // Image cache control is currently iOS only
  skip('ImageCachePolicyTest'),

  // Skip SyncMethodTest because we don't implement RNTesterTestModule and want
  // to allow web debugging
  skip('SyncMethodTest'),

  // We implement the Android AccessibilityInfo instead of AccessibilityManager
  skip('AccessibilityManagerTest'),

  // globalEvalWithSourceUrl is only set when using JSIExecutor in debug. I.e. it
  // will not work when web debugging
  skip('GlobalEvalWithSourceUrlTest'),
]);
