/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {spawn, ChildProcess} from 'child_process';
import * as fs from 'fs';

import NodeEnvironment = require('jest-environment-node');

import * as webdriverio from 'webdriverio';
import {BrowserObject, RemoteOptions} from 'webdriverio';
import {Config} from '@jest/types';

export type EnvironmentOptions = {
  app?: string;
  winAppDriverBin?: string;
  webdriverOptions?: RemoteOptions;
};

class WinAppDriverEnvironment extends NodeEnvironment {
  private readonly options: RemoteOptions;
  private readonly winappdriverBin: string;
  private winAppDriverProcess: ChildProcess | undefined;
  private browser: BrowserObject | undefined;

  constructor(config: Config.ProjectConfig) {
    super(config);
    const passedOptions: EnvironmentOptions = config.testEnvironmentOptions;

    if (!passedOptions.app) {
      throw new Error('"app" must be specified in testEnvironmentOptions');
    }

    this.winappdriverBin =
      passedOptions.winAppDriverBin ||
      'C:\\Program Files (x86)\\Windows Application Driver\\WinAppDriver.exe';

    const baseOptions: RemoteOptions = {
      port: 4723,
      capabilities: {
        app: passedOptions.app,
        // @ts-ignore
        'ms:experimental-webdriver': true,
      },
    };

    this.options = Object.assign(
      {},
      baseOptions,
      passedOptions.webdriverOptions,
    );
  }

  async setup() {
    await super.setup();

    this.winAppDriverProcess = await spawnWinAppDriver(this.winappdriverBin);
    this.browser = await webdriverio.remote(this.options);

    this.global.remote = webdriverio.remote;
    this.global.browser = this.browser;
    this.global.$ = this.browser.$.bind(this.browser);
    this.global.$$ = this.browser.$$.bind(this.browser);
  }

  async teardown() {
    if (this.browser) {
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
): Promise<ChildProcess> {
  if (!fs.existsSync(winappdriverBin)) {
    throw new Error(
      `Could not locate WinAppDriver binary at "${winappdriverBin}"`,
    );
  }

  return new Promise((resolve, reject) => {
    const process = spawn(winappdriverBin, {stdio: 'pipe'});

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

module.exports = WinAppDriverEnvironment;
