/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Supplemental, file-based logging component for test execution.
 *
 * @format
 */

import fs from '@react-native-windows/fs';

// REVIEW: Does jest have a more suitable logging facility?

class TestLog {
  constructor() {
    // this.fileName = `${__dirname}/test.${this.formatDateTime(new Date())}.log`;
    this.fileName = `${process.env.TEMP}/RNW-Debug-Test.${this.formatDateTime(
      new Date(),
    )}.log`;
  }

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

  private formatDateTime(dt: Date) {
    const YYYY = ('000' + dt.getFullYear()).slice(-4);
    const MM = ('0' + (dt.getMonth() + 1)).slice(-2);
    const DD = ('0' + dt.getDate()).slice(-2);
    const hh = ('0' + dt.getHours()).slice(-2);
    const mm = ('0' + dt.getMinutes()).slice(-2);
    const ss = ('0' + dt.getSeconds()).slice(-2);
    return `${YYYY}${MM}${DD}-${hh}${mm}${ss}`;
  }

  private formatTime(dt: Date) {
    const hh = ('0' + dt.getHours()).slice(-2);
    const mm = ('0' + dt.getMinutes()).slice(-2);
    const ss = ('0' + dt.getSeconds()).slice(-2);
    const ms = ('000' + dt.getMilliseconds()).slice(-4);
    return `${hh}:${mm}:${ss}.${ms}`;
  }

  private readonly fileName: string;
}

export const testLog = new TestLog();
