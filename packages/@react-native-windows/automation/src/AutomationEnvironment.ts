/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {execSync, spawn, ChildProcess} from 'child_process';
import fs from 'fs';
import path from 'path';

import NodeEnvironment = require('jest-environment-node');

import * as webdriverio from 'webdriverio';
import {BrowserObject, RemoteOptions} from 'webdriverio';
import {Config} from '@jest/types';
import {
  waitForConnection,
  AutomationClient,
} from '@react-native-windows/automation-channel';

export type EnvironmentOptions = {
  /**
   * The application to launch. Can be a path to an exe, or a package identity
   * name (e.g. Microsoft.WindowsAlarms)
   */
  app?: string;

  /**
   * Arguments to be passed to your application when launched
   */
  appArguments?: string;

  enableAutomationChannel?: boolean;
  automationChannelPort?: number;
  winAppDriverBin?: string;
  webdriverOptions?: RemoteOptions;
};

type AutomationChannelOptions = {
  enable: boolean;
  port: number;
};

export default class AutomationEnvironment extends NodeEnvironment {
  private readonly webDriverOptions: RemoteOptions;
  private readonly channelOptions: AutomationChannelOptions;
  private readonly winappdriverBin: string;
  private winAppDriverProcess: ChildProcess | undefined;
  private browser: BrowserObject | undefined;
  private automationClient: AutomationClient | undefined;

  constructor(config: Config.ProjectConfig) {
    super(config);
    const passedOptions: EnvironmentOptions = config.testEnvironmentOptions;

    if (!passedOptions.app) {
      throw new Error('"app" must be specified in testEnvironmentOptions');
    }

    this.winappdriverBin =
      passedOptions.winAppDriverBin ||
      path.join(
        process.env['PROGRAMFILES(X86)']!,
        'Windows Application Driver\\WinAppDriver.exe',
      );

    if (!fs.existsSync(this.winappdriverBin)) {
      throw new Error(
        `Could not find WinAppDriver at searched location: "${this.winappdriverBin}"`,
      );
    }

    const baseOptions: RemoteOptions = {
      port: 4723,
      capabilities: {
        app: resolveAppName(passedOptions.app),

        ...(passedOptions.appArguments && {
          appArguments: passedOptions.appArguments,
        }),

        // @ts-ignore
        'ms:experimental-webdriver': true,
      },
      // Level of logging verbosity: trace | debug | info | warn | error
      logLevel: 'error',

      // Default timeout for all waitFor* commands.
      waitforTimeout: 30000,

      // Default timeout in milliseconds for request
      connectionRetryTimeout: 30000,

      // Default request retries count
      connectionRetryCount: 5,
    };

    this.webDriverOptions = Object.assign(
      {},
      baseOptions,
      passedOptions.webdriverOptions,
    );

    this.webDriverOptions.capabilities = Object.assign(
      this.webDriverOptions.capabilities,
      passedOptions.webdriverOptions?.capabilities,
    );

    this.channelOptions = {
      enable: passedOptions.enableAutomationChannel === true,
      port: passedOptions.automationChannelPort || 8603,
    };
  }

  async setup() {
    await super.setup();

    this.winAppDriverProcess = await spawnWinAppDriver(
      this.winappdriverBin,
      this.webDriverOptions.port!,
    );
    this.browser = await webdriverio.remote(this.webDriverOptions);

    if (this.channelOptions.enable) {
      this.automationClient = await waitForConnection({
        port: this.channelOptions.port,
      });
      this.global.automationClient = this.automationClient;
    }

    this.global.remote = webdriverio.remote;
    this.global.browser = this.browser;
    this.global.$ = this.browser.$.bind(this.browser);
    this.global.$$ = this.browser.$$.bind(this.browser);
  }

  async teardown() {
    if (this.browser) {
      await this.browser.deleteSession();
    }

    if (this.automationClient) {
      this.automationClient.close();
    }

    this.winAppDriverProcess?.kill('SIGINT');
    await super.teardown();
  }
}

/**
 * Starts a WinAppdriver process and resolves a promise with the process once
 * it is ready to accept commands
 *
 * Inspired-by/stolen from https://github.com/licanhua/wdio-winappdriver-service
 */
async function spawnWinAppDriver(
  winappdriverBin: string,
  port: number,
): Promise<ChildProcess> {
  if (!fs.existsSync(winappdriverBin)) {
    throw new Error(
      `Could not locate WinAppDriver binary at "${winappdriverBin}"`,
    );
  }

  return new Promise((resolve, reject) => {
    const process = spawn(winappdriverBin, [port.toString()], {stdio: 'pipe'});

    process.stdout.on('data', data => {
      const s = data.toString('utf16le');
      if (s.includes('Press ENTER to exit.')) {
        resolve(process);
      } else if (s.includes('Failed to initialize')) {
        reject(new Error('Failed to start WinAppDriver: ' + s));
      }
    });

    process.stderr.once('data', err => {
      console.warn(err);
    });

    process.once('exit', exitCode => {
      reject(
        new Error(
          `WinAppDriver CLI exited before timeout (exit code: ${exitCode})`,
        ),
      );
    });
  });
}

/**
 * Convert a package identity or path to exe to the form expected by a WinAppDriver capability
 */
function resolveAppName(appName: string): string {
  if (appName.endsWith('.exe')) {
    return appName;
  }

  try {
    const packageFamilyName = execSync(
      `powershell (Get-AppxPackage -Name ${appName}).PackageFamilyName`,
    )
      .toString()
      .trim();

    if (packageFamilyName.length === 0) {
      // Rethrown below
      throw new Error();
    }

    return `${packageFamilyName}!App`;
  } catch {
    throw new Error(`Could not locate a package with identity "${appName}"`);
  }
}
