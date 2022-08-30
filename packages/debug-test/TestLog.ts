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
    fs.writeFileSync(
      this.getFileName(),
      `${this.formatTime(time)}: ${message}\n`,
      {
        flag: 'a',
      },
    );
  }

  private formatTime(dt: Date) {
    const hh = ('0' + dt.getHours()).slice(-2);
    const mm = ('0' + dt.getMinutes()).slice(-2);
    const ss = ('0' + dt.getSeconds()).slice(-2);
    const ms = ('000' + dt.getMilliseconds()).slice(-4);
    return `${hh}:${mm}:${ss}.${ms}`;
  }

  private getFileName(): string {
    if (this._fileName === '') {
      throw new Error(
        'log file name needs to get set prior to first logging call',
      );
    }
    return this._fileName;
  }

  public setFileName(fileName: string) {
    if (this._fileName === '') {
      this._fileName = fileName;
      if (fs.existsSync(this._fileName)) {
        fs.unlinkSync(this._fileName);
        this.message('a log file with this path existed and was overwritten');
      }
    } else {
      throw new Error(
        `log file name is being reassigned, old value "${this._fileName}", new value "${fileName}"`,
      );
    }
  }

  private _fileName: string = '';
})();
