/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {Logger} from './types';

function stamp(): string {
  return new Date().toISOString().replace('T', ' ').replace('Z', '');
}

export function createLogger(verbose: boolean): Logger {
  return {
    info(message) {
      process.stdout.write(`[${stamp()}] ${message}\n`);
    },
    warn(message) {
      process.stdout.write(`[${stamp()}] WARN  ${message}\n`);
    },
    error(message) {
      process.stderr.write(`[${stamp()}] ERROR ${message}\n`);
    },
    debug(message) {
      if (verbose) process.stdout.write(`[${stamp()}] debug ${message}\n`);
    },
  };
}
