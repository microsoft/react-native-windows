/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import chalk from 'chalk';
import {spawnSync, spawn, ChildProcess} from 'child_process';
import fs from '@react-native-windows/fs';
import path from 'path';
import readlineSync from 'readline-sync';

import NodeEnvironment from 'jest-environment-node';
import * as webdriverio from 'webdriverio';
import {BrowserObject, RemoteOptions} from 'webdriverio';
import {JestEnvironmentConfig} from '@jest/environment';
import type {EnvironmentContext} from '@jest/environment';
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
   * Instead of letting WinAppDriver launch and attach to the app directly,
   * create a Root (Desktop) session and search for the app's window.
   *
   * Note: This is only really necessary to correctly attach to packaged
   * WinAppSDK apps.
   */
  useRootSession?: boolean;

  /**
   * When using a Root (Desktop) session, still launch the test app during setup
   * and close it during cleanup.
   *
   * Defaults to true when using `useRootSession` to mimic the expected test
   * behavior, but can be disabled if you're trying to test an already
   * running app instance.
   */
  rootLaunchApp?: boolean;

  /**
   * Arguments to be passed to your application when launched
   */
  appArguments?: string;

  appWorkingDir?: string;

  enableAutomationChannel?: boolean;
  automationChannelPort?: number;
  winAppDriverBin?: string;
  breakOnStart?: boolean;
  webdriverOptions?: RemoteOptions;
};

type AutomationChannelOptions = {
  enable: boolean;
  port: number;
};

export default class AutomationEnvironment extends NodeEnvironment {
  private readonly rootWebDriverOptions?: RemoteOptions;
  private readonly webDriverOptions: RemoteOptions;
  private readonly channelOptions: AutomationChannelOptions;
  private readonly winappdriverBin: string;
  private readonly breakOnStart: boolean;
  private readonly useRootSession: boolean;
  private readonly rootLaunchApp: boolean;
  private winAppDriverProcess: ChildProcess | undefined;
  private browser: BrowserObject | undefined;
  private automationClient: AutomationClient | undefined;

  constructor(config: JestEnvironmentConfig, context: EnvironmentContext) {
    super(config, context);
    const passedOptions: EnvironmentOptions =
      config.projectConfig.testEnvironmentOptions;

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
      hostname: '127.0.0.1',
      port: 4723,

      // Level of logging verbosity: trace | debug | info | warn | error
      logLevel: 'error',

      // Default timeout for all waitFor* commands.
      waitforTimeout: 30000,

      // Default timeout in milliseconds for request
      connectionRetryTimeout: 30000,

      // Default request retries count
      connectionRetryCount: 5,
    };

    this.useRootSession = !!passedOptions.useRootSession;
    this.rootLaunchApp =
      passedOptions.rootLaunchApp === undefined
        ? this.useRootSession
        : !!passedOptions.rootLaunchApp;

    if (this.useRootSession) {
      this.rootWebDriverOptions = Object.assign(
        {},
        baseOptions,
        {
          capabilities: {
            app: 'Root',
            // @ts-ignore
            'ms:experimental-webdriver': true,
          },
        },
        passedOptions.webdriverOptions,
      );

      this.webDriverOptions = Object.assign(
        {},
        baseOptions,
        {
          capabilities: {
            // Save the name for now, we'll get the handle later
            appTopLevelWindow: passedOptions.app,
            // @ts-ignore
            'ms:experimental-webdriver': true,
          },
        },
        passedOptions.webdriverOptions,
      );
    } else {
      this.webDriverOptions = Object.assign(
        {},
        baseOptions,
        {
          capabilities: {
            app: resolveAppName(passedOptions.app),
            ...(passedOptions.appWorkingDir && {
              appWorkingDir: passedOptions.appWorkingDir,
            }),
            ...(passedOptions.appArguments && {
              appArguments: passedOptions.appArguments,
            }),

            // @ts-ignore
            'ms:experimental-webdriver': true,
          },
        },
        passedOptions.webdriverOptions,
      );
    }

    this.webDriverOptions.capabilities = Object.assign(
      this.webDriverOptions.capabilities!,
      passedOptions.webdriverOptions?.capabilities,
    );

    this.channelOptions = {
      enable: passedOptions.enableAutomationChannel === true,
      port: passedOptions.automationChannelPort || 8603,
    };

    this.breakOnStart = passedOptions.breakOnStart === true;
  }

  async setup() {
    await super.setup();
    this.winAppDriverProcess = await spawnWinAppDriver(
      this.winappdriverBin,
      this.webDriverOptions.port!,
    );

    if (this.useRootSession) {
      // Extract out the saved window name
      const appName = (this.webDriverOptions.capabilities! as any)
        .appTopLevelWindow;

      if (this.rootLaunchApp) {
        const appPackageName = resolveAppName(appName);
        spawnSync('cmd', [
          '/c',
          'start',
          `shell:AppsFolder\\${appPackageName}`,
        ]);
      }

      // Set up the "Desktop" or Root session
      const rootBrowser = await webdriverio.remote(this.rootWebDriverOptions);

      // Get the list of windows
      const allWindows = await rootBrowser.$$('//Window');

      // Find our target window
      let appWindow: webdriverio.Element | undefined;
      for (const window of allWindows) {
        if ((await window.getAttribute('Name')) === appName) {
          appWindow = window;
          break;
        }
      }

      if (!appWindow) {
        throw new Error(`Unable to find window with Name === '${appName}'.`);
      }

      // Swap the the window handle for WinAppDriver
      const appWindowHandle = parseInt(
        await appWindow!.getAttribute('NativeWindowHandle'),
        10,
      );

      (this.webDriverOptions.capabilities as any).appTopLevelWindow =
        '0x' + appWindowHandle.toString(16);

      await rootBrowser.deleteSession();
    }

    this.browser = await webdriverio.remote(this.webDriverOptions);

    if (this.breakOnStart) {
      readlineSync.question(
        chalk.bold.yellow('Breaking before tests start\n') +
          'Press Enter to resume...',
      );
    }

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
    if (this.automationClient) {
      this.automationClient.close();
    }

    if (this.browser) {
      if (this.rootLaunchApp) {
        // We started the app, so let's close it too
        await this.browser.closeWindow();
      }
      await this.browser.deleteSession();
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
    const packageFamilyName = spawnSync('powershell', [
      `(Get-AppxPackage -Name ${appName}).PackageFamilyName`,
    ])
      .stdout.toString()
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
