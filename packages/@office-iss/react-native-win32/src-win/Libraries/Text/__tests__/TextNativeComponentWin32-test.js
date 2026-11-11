/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

jest.mock('../../../src/private/featureflags/ReactNativeFeatureFlags', () => ({
  ...jest.requireActual(
    '../../../src/private/featureflags/ReactNativeFeatureFlags',
  ),
  enablePreparedTextLayout: () => true,
}));

describe('TextNativeComponentWin32', () => {
  it('uses RCTText for selectable text even when prepared text layout is enabled', () => {
    const {
      NativeSelectableText,
      NativeText,
    } = require('../TextNativeComponent.win32');

    expect(NativeSelectableText).toBe(NativeText);
  });
});
