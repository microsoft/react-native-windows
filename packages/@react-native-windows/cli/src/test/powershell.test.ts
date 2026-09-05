/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {EventEmitter} from 'events';
import type {
  EnvironmentInfo,
  Loader,
} from '@react-native-community/cli-doctor/build/types';

jest.mock('@react-native-windows/find-dotnet-tools', () => ({
  findPowerShell: jest.fn(),
}));
jest.mock('../utils/pathHelpers', () => ({
  resolveRnwRoot: () => '/project/node_modules/react-native-windows',
}));
jest.mock('child_process', () => ({
  ...jest.requireActual('child_process'),
  spawn: jest.fn(),
}));
jest.mock('execa', () => jest.fn());
jest.mock('ora', () =>
  jest.fn(() => ({
    start: jest.fn().mockReturnThis(),
    stop: jest.fn(),
    succeed: jest.fn(),
    fail: jest.fn(),
  })),
);

const originalPlatform = Object.getOwnPropertyDescriptor(process, 'platform')!;
const powershellPath = 'C:\\Program Files\\PowerShell\\7\\pwsh.exe';
const missingPowerShell = new Error('Unable to find pwsh.exe.');
const environmentInfo = {} as EnvironmentInfo;

let findPowerShell: jest.Mock;
let spawn: jest.Mock;
let execa: jest.Mock;

beforeEach(() => {
  jest.resetModules();
  findPowerShell = jest.requireMock(
    '@react-native-windows/find-dotnet-tools',
  ).findPowerShell;
  findPowerShell.mockImplementation(() => {
    throw missingPowerShell;
  });
  spawn = jest.requireMock('child_process').spawn;
  spawn.mockImplementation(() => {
    const child = new EventEmitter();
    queueMicrotask(() => child.emit('close', 0));
    return child;
  });
  execa = jest.requireMock('execa');
  execa.mockResolvedValue({exitCode: 0});
});

afterEach(() => {
  Object.defineProperty(process, 'platform', originalPlatform);
});

describe.each(['darwin', 'win32'])('PowerShell discovery on %s', platform => {
  beforeEach(() => {
    Object.defineProperty(process, 'platform', {value: platform});
  });

  test('imports the CLI and serializes health checks without PowerShell', () => {
    const cli: typeof import('../index') = require('../index');

    expect(cli.commands.map(command => command.name)).toContain('run-windows');
    expect(typeof cli.projectConfig).toBe('function');
    expect(typeof cli.dependencyConfig).toBe('function');
    // RNX Kit serializes react-native.config.js, invoking its healthChecks getter.
    expect(() =>
      JSON.stringify({
        get healthChecks() {
          return cli.getHealthChecks();
        },
      }),
    ).not.toThrow();
    expect(findPowerShell).not.toHaveBeenCalled();
    expect(spawn).not.toHaveBeenCalled();
    expect(execa).not.toHaveBeenCalled();
  });

  test('reports missing PowerShell only when a PowerShell command is invoked', async () => {
    const {
      runPowerShellScriptFunction,
    } = require('../utils/commandWithProgress');

    await expect(
      runPowerShellScriptFunction(
        'Run script',
        null,
        'Get-Date',
        true,
        'Unknown',
      ),
    ).rejects.toBe(missingPowerShell);
    expect(findPowerShell).toHaveBeenCalledTimes(1);
    expect(spawn).not.toHaveBeenCalled();
  });

  test('uses the discovered executable and caches successful discovery', async () => {
    findPowerShell.mockReturnValue(powershellPath);
    const {
      runPowerShellScriptFunction,
    } = require('../utils/commandWithProgress');

    for (let i = 0; i < 2; i++) {
      await runPowerShellScriptFunction(
        'Run script',
        null,
        'Get-Date',
        true,
        'Unknown',
      );
    }
    expect(findPowerShell).toHaveBeenCalledTimes(1);
    expect(spawn).toHaveBeenCalledTimes(2);
    expect(spawn).toHaveBeenCalledWith(
      powershellPath,
      expect.arrayContaining(['-NoProfile', '-Command']),
      {stdio: 'inherit'},
    );
  });

  test('retries discovery after a failure', async () => {
    const {
      runPowerShellScriptFunction,
    } = require('../utils/commandWithProgress');
    await expect(
      runPowerShellScriptFunction(
        'Run script',
        null,
        'Get-Date',
        true,
        'Unknown',
      ),
    ).rejects.toBe(missingPowerShell);

    findPowerShell.mockReturnValue(powershellPath);
    await runPowerShellScriptFunction(
      'Run script',
      null,
      'Get-Date',
      true,
      'Unknown',
    );
    expect(findPowerShell).toHaveBeenCalledTimes(2);
    expect(spawn).toHaveBeenCalledTimes(1);
  });
});

describe('Windows health checks', () => {
  beforeEach(() => {
    Object.defineProperty(process, 'platform', {value: 'win32'});
  });

  function getFirstHealthCheck() {
    const {
      getHealthChecks,
    }: typeof import('../commands/healthCheck/healthChecks') = require('../commands/healthCheck/healthChecks');
    return getHealthChecks()![0].healthchecks[0];
  }

  test('reports a failed diagnostic when PowerShell is missing', async () => {
    const check = getFirstHealthCheck();
    expect(findPowerShell).not.toHaveBeenCalled();

    await expect(check.getDiagnostics(environmentInfo)).resolves.toEqual({
      needsToBeFixed: true,
    });
    expect(findPowerShell).toHaveBeenCalledTimes(1);
    expect(execa).not.toHaveBeenCalled();
  });

  test('uses PowerShell for diagnostics and automatic fixes', async () => {
    findPowerShell.mockReturnValue(powershellPath);
    const check = getFirstHealthCheck();
    const loader = {succeed: jest.fn(), fail: jest.fn()} as unknown as Loader;
    const logManualInstallation = jest.fn();
    expect(findPowerShell).not.toHaveBeenCalled();

    await expect(check.getDiagnostics(environmentInfo)).resolves.toEqual({
      needsToBeFixed: false,
    });
    await check.runAutomaticFix({
      loader,
      logManualInstallation,
      environmentInfo,
    });

    expect(findPowerShell).toHaveBeenCalledTimes(1);
    expect(execa).toHaveBeenNthCalledWith(
      1,
      expect.stringContaining(`"${powershellPath}"`),
    );
    expect(execa).toHaveBeenNthCalledWith(
      2,
      expect.stringContaining(`"${powershellPath}"`),
      {stdio: 'inherit'},
    );
    expect(loader.succeed).toHaveBeenCalledTimes(1);
    expect(loader.fail).not.toHaveBeenCalled();
    expect(logManualInstallation).not.toHaveBeenCalled();
  });
});
