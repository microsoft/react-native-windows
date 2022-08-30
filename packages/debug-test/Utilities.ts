/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Utility functions.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import {testLog} from './TestLog';

/**
 * Pauses execution for the specified amount of time.
 * @param ms Time, in milliseconds, to pause execution.
 * @returns Promise which, when awaited, pauses execution.
 */
export async function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

/**
 * Abbreviates a string.
 * @param s String to abbreviate
 * @param maxLength Length to which to abbreviate the string.
 * @returns Abbreviated string.
 */
export function abbreviate(s: string, maxLength: number): string {
  if (maxLength < 4)
    throw new Error('shortenIfNeeded requires minimum length of 4');
  if (s.length <= maxLength) {
    return s;
  }
  return s.slice(0, maxLength - /* length of "..." */ 3) + '...';
}

/**
 * Formats the given date object as a "YYYYMMDD-hhmmss" string.
 * @param dt Date to format.
 * @returns String representation of the data.
 */
export function formatDateTime(dt: Date) {
  const YYYY = ('000' + dt.getFullYear()).slice(-4);
  const MM = ('0' + (dt.getMonth() + 1)).slice(-2);
  const DD = ('0' + dt.getDate()).slice(-2);
  const hh = ('0' + dt.getHours()).slice(-2);
  const mm = ('0' + dt.getMinutes()).slice(-2);
  const ss = ('0' + dt.getSeconds()).slice(-2);
  return `${YYYY}${MM}${DD}-${hh}${mm}${ss}`;
}

export async function saveScreenShot(fileNameSuffix?: string) {
  let fileNameBase = process.env.DEBUGTEST_LOGFOLDER
    ? `${process.env.DEBUGTEST_LOGFOLDER}\\Screenshot`
    : `${process.env.TEMP}\\Screenshot`;

  if (fileNameSuffix) {
    fileNameBase += ` - ${fileNameSuffix}`;
  }

  let fileNameCandidate = `${fileNameBase}.png`;
  for (let i = 2; i < 10000; ++i) {
    if (!fs.existsSync(fileNameCandidate)) {
      testLog.message(`saving screenshot as "${fileNameCandidate}"`);
      return global.browser.saveScreenshot(fileNameCandidate);
    }
    fileNameCandidate = `${fileNameBase} (${i}).png`;
  }

  return Promise.reject(
    new Error('could not find a file name to save screenshot under'),
  );
}
