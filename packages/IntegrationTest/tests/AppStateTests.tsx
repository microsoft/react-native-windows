/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {assert} from 'chai';
import {AppState} from 'react-native';
import {functionTest} from './lib/TestDefinition';

/**
 * Verify AppState.currentState
 */
functionTest('AppStateCurrentState', () => {
  assert.equal(AppState.currentState, 'active');
});
