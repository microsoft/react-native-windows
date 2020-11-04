/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as path from 'path';
import {randomBytes} from 'crypto';
import * as appInsights from 'applicationinsights';

appInsights.setup('795006ca-cf54-40ee-8bc6-03deb91401c3');
export const telClient = appInsights.defaultClient;

// CODE-SYNC: \packages\react-native-windows-init\src\Cli.ts

if (process.env.RNW_CLI_TEST) {
  telClient.commonProperties.isTest = process.env.RNW_CLI_TEST;
}

if (!telClient.commonProperties.sessionId) {
  telClient.commonProperties.sessionId = randomBytes(16).toString('hex');

  /**
   * Sanitize any paths that appear between quotes (''), brackets ([]), or double quotes ("").
   * @param msg the string to sanitize
   */
  function sanitizeMessage(msg: string): string {
    const parts = msg.split(/['[\]"]/g);
    const clean = [];
    const projectRoot = process.cwd().toLowerCase();
    for (const part of parts) {
      if (part.toLowerCase().startsWith(projectRoot)) {
        const ext = path.extname(part);
        const rest = part.slice(projectRoot.length);
        // this is in the project dir but not under node_modules
        if (rest.toLowerCase().startsWith('\\windows\\')) {
          clean.push(`[windows]\\???${ext}(${part.length})`);
        } else if (rest.toLowerCase().startsWith('\\node_modules\\')) {
          clean.push(rest.slice(1));
        } else {
          clean.push(`[project_dir]\\???${ext}(${part.length})`);
        }
      } else {
        clean.push(part);
      }
    }
    return clean.join(' ');
  }

  /**
   * Remove PII from exceptions' stack traces and messages
   * @param envelope the telemetry envelope. Provided by AppInsights.
   */
  function sanitizeExceptions(envelope: any /*context: any*/): boolean {
    if (envelope.data.baseType === 'ExceptionData') {
      const data = envelope.data.baseData;
      for (const exception of data.exceptions || []) {
        for (const frame of exception.parsedStack) {
          const parens = frame.method.indexOf('(');
          if (parens !== -1) {
            // case 1: method === 'methodName (rootOfThePath'
            frame.method = frame.method.substr(0, parens).trim();
          } else {
            // case 2: method === <no_method> or something without '(', fileName is full path
          }
          // preserve only the last_directory/filename
          frame.fileName = path.join(
            path.basename(path.dirname(frame.fileName)),
            path.basename(frame.fileName),
          );
          frame.assembly = '';
        }

        exception.message = sanitizeMessage(exception.message);
      }
    }
    return true;
  }

  telClient.addTelemetryProcessor(sanitizeExceptions);
}
