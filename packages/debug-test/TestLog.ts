/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Supplemental, file-based logging component for test execution.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import {formatDateTime} from './Utilities';

// REVIEW: Does jest have a more suitable logging facility?

export const testLog = new (class {
  /**
   * Logs a message.
   * @param message Message to log.
   */
  public message(message: string) {
    this.write(message);
  }

  /**
   * Logs a warning.
   * @param message Warning to log
   */
  public warning(message: string) {
    this.write(`WARNING: ${message}`);
  }

  /**
   * Logs an error.
   * @param message Error to log
   */
  public error(message: string) {
    this.write(`ERROR: ${message}`);
  }

  private write(message: string) {
    // console.log(message);
    const time = new Date();
    fs.writeFileSync(this.fileName, `${this.formatTime(time)}: ${message}\n`, {
      flag: 'a',
    });
  }

  private formatTime(dt: Date) {
    const hh = ('0' + dt.getHours()).slice(-2);
    const mm = ('0' + dt.getMinutes()).slice(-2);
    const ss = ('0' + dt.getSeconds()).slice(-2);
    const ms = ('000' + dt.getMilliseconds()).slice(-4);
    return `${hh}:${mm}:${ss}.${ms}`;
  }

  /**
   * Path and name of the test log file. Default value intended to be overwritten by a test-suite-specific name.
   */
  public fileName: string = `${process.env.TEMP}/test.${formatDateTime(
    new Date(),
  )}.log`;
})();
