/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import {totalmem, cpus} from 'os';

import ci from 'ci-info';
import {randomBytes} from 'crypto';
import {machineId} from 'node-machine-id';
import osLocale from 'os-locale';

/**
 * Gets a telemetry-safe stable device ID.
 * @returns A telemetry-safe stable device ID.
 */
export async function deviceId(): Promise<string> {
  return await machineId(false);
}

/**
 * Gets the device locale.
 * @returns The device locale.
 */
export async function deviceLocale(): Promise<string> {
  return await osLocale();
}

/**
 * Gets the device's number of CPUs.
 * @returns The device's number of CPUs.
 */
export function deviceNumCPUs(): number {
  return cpus().length;
}

/**
 * Gets the device's total memory in bytes.
 * @returns The device's total memory in bytes.
 */
export function deviceTotalMemory(): number {
  return totalmem();
}

/**
 * Gets the free space of the give drive in bytes.
 * @param drivePath A path on the drive to check.
 * @returns The free space of the give drive in bytes.
 */
export function deviceDiskFreeSpace(drivePath?: string | null): number {
  try {
    const out = execSync(`dir /-C ${drivePath ?? process.cwd()}`)
      .toString()
      .split('\r\n');
    const line = out[out.length - 2];
    const result = line.match(/(\d+) [^\d]+(\d+) /);
    if (result && result.length > 2) {
      return Number(result[2]);
    }
  } catch {}
  return -1;
}

/**
 * Gets the telemetry sample rate.
 * @returns The telemetry sample rate.
 */
export function sampleRate(): number {
  return 100;
}

/**
 * Gets whether or not telemetry events are captured when running in CI.
 * @returns Whether or not telemetry events are captured when running in CI.
 */
export function captureCI(): boolean {
  // Only capture events in CI if running tests
  return isCliTest();
}

/**
 * Gets the whether the process is currently running in CI.
 * @returns Whether the process is currently running in CI.
 */
export function isCI(): boolean {
  return ci.isCI;
}

/**
 * Gets the type of CI the process is running under.
 * @returns The type of CI the process is running under.
 */
export function ciType(): string {
  return ci.isCI ? ci.name ?? 'Unknown' : 'None';
}

/**
 * Gets whether the process is running on a Microsoft owned machine.
 * @returns Whether the process is running on a Microsoft owned machine.
 */
export function isMsftInternal(): boolean {
  return (
    process.env.UserDNSDomain !== undefined &&
    process.env.UserDNSDomain.toLowerCase().endsWith('corp.microsoft.com')
  );
}

/**
 * Gets whether the process is running as part of our CLI tests.
 * @returns Whether the process is running as part of our CLI tests.
 */
export function isCliTest(): boolean {
  return (
    process.env.RNW_CLI_TEST !== undefined &&
    process.env.RNW_CLI_TEST.toLowerCase().trim() === 'true'
  );
}

let sessionId: string | undefined;

/**
 * Gets a stable session ID for correlating telemetry events.
 * @returns A stable session ID for correlating telemetry events.
 */
export function getSessionId(): string {
  return (
    sessionId ??
    (sessionId = randomBytes(16)
      .toString('hex')
      .padStart(32, '0'))
  );
}
