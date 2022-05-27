/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Utility functions.
 *
 * @format
 */

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
