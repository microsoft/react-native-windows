/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

function loadAccessibilityInfo(platform, nativeManager) {
  jest.doMock('../../../Utilities/Platform', () => ({
    ...jest.requireActual('../../../Utilities/Platform').default,
    OS: platform,
  }));
  jest.doMock('../NativeAccessibilityManager', () => nativeManager);
  return jest.requireActual('../AccessibilityInfo.windows').default;
}

describe('AccessibilityInfo.isShowBordersEnabled', () => {
  beforeEach(() => {
    jest.resetModules();
  });

  it.each(['windows', 'android'])(
    'resolves false on %s without an iOS accessibility manager',
    async platform => {
      const AccessibilityInfo = loadAccessibilityInfo(platform, null);

      await expect(AccessibilityInfo.isShowBordersEnabled()).resolves.toBe(false);
    },
  );

  it.each(['windows', 'android'])(
    'does not query the iOS accessibility manager on %s',
    async platform => {
      const getCurrentShowBordersState = jest.fn(resolve => resolve(true));
      const AccessibilityInfo = loadAccessibilityInfo(platform, {
        getCurrentShowBordersState,
      });

      await expect(AccessibilityInfo.isShowBordersEnabled()).resolves.toBe(false);
      expect(getCurrentShowBordersState).not.toHaveBeenCalled();
    },
  );

  it.each([true, false])('preserves the iOS native value %s', async enabled => {
    const getCurrentShowBordersState = jest.fn(resolve => resolve(enabled));
    const AccessibilityInfo = loadAccessibilityInfo('ios', {
      getCurrentShowBordersState,
    });

    await expect(AccessibilityInfo.isShowBordersEnabled()).resolves.toBe(enabled);
    expect(getCurrentShowBordersState).toHaveBeenCalledTimes(1);
  });

  it('preserves iOS native rejection', async () => {
    const error = new Error('Native query failed');
    const AccessibilityInfo = loadAccessibilityInfo('ios', {
      getCurrentShowBordersState: jest.fn((resolve, reject) => reject(error)),
    });

    await expect(AccessibilityInfo.isShowBordersEnabled()).rejects.toBe(error);
  });

  it.each([null, {}])(
    'rejects on iOS when the native method is unavailable (%s)',
    async nativeManager => {
      const AccessibilityInfo = loadAccessibilityInfo('ios', nativeManager);

      await expect(AccessibilityInfo.isShowBordersEnabled()).rejects.toThrow(
        'NativeAccessibilityManagerIOS.getCurrentShowBordersState is not available',
      );
    },
  );
});
