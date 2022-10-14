/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {ChildProcess, fork} from 'child_process';
import * as path from 'path';
import {testLog} from './TestLog';

export class Metro {
  async start(): Promise<void> {
    return new Promise((resolve, _reject) => {
      // TODO: check if Metro is already running
      //
      // If metro is already running, we get
      //     error listen EADDRINUSE: address already in use :::8081.
      //     Error: listen EADDRINUSE: address already in use :::8081
      //         at Server.setupListenHandle [as _listen2] (node:net:1330:16)
      //         at listenInCluster (node:net:1378:12)
      // Thus, can we check if port 8081 is already in use?

      const repoRootDir = path.normalize(`${__dirname}\\..\\..`);
      const metroScript = `${repoRootDir}\\node_modules\\react-native\\cli.js`;
      const metroCurrentDir = `${repoRootDir}\\packages\\playground`;

      testLog.message(`Metro script "${metroScript}"`);
      testLog.message(`Metro current directory "${metroCurrentDir}"`);

      this.metroProcess = fork(metroScript, ['start', '--no-interactive'], {
        cwd: metroCurrentDir,
        stdio: 'pipe',
      });

      testLog.message(`Metro process ID ${this.metroProcess.pid}`);

      // keep an eye on Metro
      this.metroProcess.stdout!.addListener('data', Metro.logMetroOutput);

      // trying to sync with Metro being ready to serve packages
      const findReadyMessage = (data: any) => {
        const s = data.toString();
        if (s.includes('Welcome to Metro')) {
          testLog.message(`Metro appears to be ready`);
          this.metroProcess!.stdout!.removeListener('data', findReadyMessage);
          resolve();
        }
      };

      this.metroProcess.stdout!.addListener('data', findReadyMessage);
    });
  }

  public stop() {
    if (this.metroProcess === null)
      throw new Error(
        "metroProcess field is null, has 'start' method not been called?",
      );

    // REVIEW: Better way of ending Metro?
    this.metroProcess.removeListener('data', Metro.logMetroOutput);
    const killResult = this.metroProcess.kill('SIGINT');
    testLog.message(`stopped Metro (${killResult})`);
  }

  public isBundleServed(bundleName: string): Promise<void> {
    return new Promise<void>((resolve, _reject) => {
      const findBundleName = (data: any) => {
        const s = data.toString();
        if (s.includes(bundleName)) {
          testLog.message(`Metro appears to serve the "${bundleName}" bundle`);
          this.metroProcess!.stdout!.removeListener('data', findBundleName);
          resolve();
        }
      };

      this.metroProcess!.stdout!.addListener('data', findBundleName);
    });
  }

  private static logMetroOutput(data: any) {
    const s = data.toString();
    testLog.message(`Metro stdout "${s}"`);
  }

  private metroProcess: ChildProcess | null = null;
}
