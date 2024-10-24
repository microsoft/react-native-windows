/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as basePropUtils from '../utils/basePropUtils';

test('deviceId() is valid', async () => {
  const value = await basePropUtils.deviceId();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('deviceId() does not change', async () => {
  expect(await basePropUtils.deviceId()).toBe(await basePropUtils.deviceId());
});

test('deviceArchitecture() is valid', () => {
  const value = basePropUtils.deviceArchitecture();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('nodeArchitecture() is valid', () => {
  const value = basePropUtils.nodeArchitecture();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('deviceClass() is valid', () => {
  const value = basePropUtils.deviceClass();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('nodePlatform() is valid', () => {
  const value = basePropUtils.nodePlatform();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('deviceLocale() is valid', async () => {
  const value = await basePropUtils.deviceLocale();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('deviceLocale() does not change', async () => {
  expect(await basePropUtils.deviceLocale()).toBe(
    await basePropUtils.deviceLocale(),
  );
});

test('deviceNumCPUs() is valid', () => {
  const value = basePropUtils.deviceNumCPUs();
  expect(value).toBeGreaterThan(0);
});

test('deviceTotalMemory() is valid', () => {
  const value = basePropUtils.deviceTotalMemory();
  expect(value).toBeGreaterThan(0);
});

test('deviceDiskFreeSpace() is valid', () => {
  const value = basePropUtils.deviceDiskFreeSpace();
  expect(value).toBeGreaterThanOrEqual(0);
});

test('ciType() is valid', () => {
  const value = basePropUtils.ciType();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});

test('ciType() is None when not in CI', () => {
  if (basePropUtils.isCI()) {
    expect(basePropUtils.ciType()).not.toBe('None');
  } else {
    expect(basePropUtils.ciType()).toBe('None');
  }
});

test('isMsftInternal() is false with no domain', () => {
  delete process.env.UserDNSDomain;
  expect(basePropUtils.isMsftInternal()).toBe(false);
});

test('isMsftInternal() is false with example.com domain', () => {
  process.env.UserDNSDomain = 'example.com';
  expect(basePropUtils.isMsftInternal()).toBe(false);
});

test('isMsftInternal() is true with Msft domain', () => {
  process.env.UserDNSDomain = 'test.corp.microsoft.com';
  expect(basePropUtils.isMsftInternal()).toBe(true);
});

test('isCliTest() is true if RNW_CLI_TEST env variable is set', () => {
  process.env.RNW_CLI_TEST = 'true';
  expect(basePropUtils.isCliTest()).toBe(true);
});

test('isCliTest() is false if no RNW_CLI_TEST variable', () => {
  delete process.env.RNW_CLI_TEST;
  expect(basePropUtils.isCliTest()).toBe(false);
});

test('getSessionId() is valid', () => {
  const value = basePropUtils.getSessionId();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
  expect(value).toHaveLength(32);
});

test('getSessionId() is a guid', () => {
  const value = basePropUtils.getSessionId();
  expect(value).toHaveLength(32);

  const parseGuid = () => {
    parseInt(value, 16);
  };

  expect(parseGuid).not.toThrow();
});

test('getSessionId() does not change', () => {
  expect(basePropUtils.getSessionId()).toBe(basePropUtils.getSessionId());
});

test('fullBuildInfo() is valid', () => {
  const value = basePropUtils.fullBuildInfo();
  expect(value).toBeDefined();
  expect(value).not.toBe('');
  expect(value).not.toBeNull();
});
