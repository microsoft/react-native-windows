/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {execSync} from 'child_process';
import {totalmem, cpus, platform} from 'os';

import ci from 'ci-info';
import {randomBytes} from 'crypto';
import osLocale from 'os-locale';

const DeviceIdRegPath = 'HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\SQMClient';
const DeviceIdRegKey = 'MachineId';

const DeviceIdBuildPath =
  '"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"';
const DeviceIdBuildKey = 'BuildLabEx';

/**
 * Given a path and a key, retrieves the value from the Registry.
 * @returns If the path and key exist, the requested value from the Registry; empty string otherwise.
 */
export async function getValueFromRegistry(
  path: string,
  key: string,
): Promise<string> {
  try {
    let regCommand = `${process.env.windir}\\System32\\reg.exe query ${path} /v ${key}`;
    if (deviceArchitecture() === 'x64') {
      // Ensure we query the correct registry
      regCommand += ' /reg:64';
    }

    const output = execSync(regCommand).toString();
    return output;
  } catch {}

  return '';
}

/**
 * Gets a telemetry-safe stable device ID.
 * @returns A telemetry-safe stable device ID.
 */
export async function deviceId(): Promise<string> {
  try {
    const deviceIdValue = await getValueFromRegistry(
      DeviceIdRegPath,
      DeviceIdRegKey,
    );

    const result = deviceIdValue.match(/\{([0-9A-Fa-f-]{36})\}/);
    if (result && result.length > 1) {
      return `s:${result[1]}`;
    }
  } catch {}
  return '';
}

/**
 * Gets the Windows build name, number and architecture.
 * @returns A string containing the Windows build name, number and architecture.
 * e.g. 19569.1000.amd64fre.rs_prerelease.200214-1419
 */
export async function fullBuildInfo(): Promise<string> {
  try {
    const fullBuildValue = await getValueFromRegistry(
      DeviceIdBuildPath,
      DeviceIdBuildKey,
    );

    // Retrieve the build info
    const match = fullBuildValue.match(/BuildLabEx\s+REG_SZ\s+([^\r\n]+)/);
    if (match && match.length > 1) {
      return match[1];
    }
  } catch {}
  return '';
}

/**
 * Gets the device architecture, like x86/x64/arm64.
 * @returns The device architecture.
 */
export function deviceArchitecture(): string {
  const nodeArch = nodeArchitecture();

  // Check if we're running x86 node on x64 hardware
  if (nodeArch === 'x86' && process.env.PROCESSOR_ARCHITEW6432 === 'AMD64') {
    return 'x64';
  }

  return nodeArch;
}

/**
 * Gets the node architecture, like x86/x64/arm64.
 * @returns The node architecture.
 */
export function nodeArchitecture(): string {
  return process.arch === 'ia32' ? 'x86' : process.arch;
}

/**
 * Gets the node platform, like darwin/linux/win32.
 * @returns The device platform.
 */
export function nodePlatform(): string {
  return platform();
}

/**
 * Gets the OS name, to be filled in the PartA device.deviceClass field.
 * @returns The device class.
 */
export function deviceClass(): string {
  const node = nodePlatform();

  switch (node) {
    case 'darwin':
      return 'Mac';
    case 'linux':
      return 'Linux';
    case 'win32':
      return 'Windows';
    default:
      return node;
  }
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
    sessionId ?? (sessionId = randomBytes(16).toString('hex').padStart(32, '0'))
  );
}
