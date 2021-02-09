/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * The logger used by the Doxysaurus project.
 *
 * The goal is make the logging look simpler in the code.
 * - Text in square brackets [] is rendered in green.
 * - Text in curly brackets {} is rendered in bright cyan.
 * - Repeat [] or {} brackets to render them. E.g. {{, ]], }}, ]].
 * - Provide the second objTree parameter to dump a JavaScrip object.
 * - Setting `log.quiet = true` disables console output.
 */

import chalk from 'chalk';
import fs from 'fs';
import path from 'path';
import util from 'util';

export interface LoggerOptions {
  quiet?: boolean;
  logFile?: string;
}

export interface WriteLog {
  (message: string, obj?: any): void;
}

export interface Logger extends WriteLog {
  init: (options: LoggerOptions) => void;
  verbose: WriteLog;
  warning: WriteLog;
  error: WriteLog;
  options: LoggerOptions;
}

type LogLevel = 'info' | 'warning' | 'error';

function writeLog(logLevel: LogLevel, message: string, objTree?: any) {
  const hasError = logLevel === 'error';

  const logFilePath = log.options.logFile
    ? path.join(process.cwd(), log.options.logFile)
    : undefined;
  const consoleOutput: string[] | undefined =
    hasError || !log.options.quiet ? [] : undefined;
  const logFileOutput: string[] | undefined = logFilePath ? [] : undefined;

  if (hasError) {
    write('Error: ', chalk.redBright);
  } else if (logLevel === 'warning') {
    write('Warning: ', chalk.yellow);
  }

  const openBrackets = '[{';
  const closeBrackets = ']}';
  let bracketIndex = -1;
  let index = 0;
  for (let i = index, ch = '\0'; (ch = message[i]); ++i) {
    if (ch === message[i + 1]) {
      if (openBrackets.includes(ch) || closeBrackets.includes(ch)) {
        write(message.slice(index, i));
        index = ++i;
      }
    } else if (bracketIndex >= 0 && ch === closeBrackets[bracketIndex]) {
      if (bracketIndex === 0) {
        write(message.slice(index, i), chalk.greenBright);
      } else if (bracketIndex === 1) {
        write(message.slice(index, i), chalk.cyanBright);
      }
      bracketIndex = -1;
      index = i + 1;
    } else {
      const tempBraceIndex = openBrackets.indexOf(ch);
      if (tempBraceIndex === -1) {
        continue;
      }

      write(message.slice(index, i));
      bracketIndex = tempBraceIndex;
      index = i + 1;
    }
  }
  write(message.slice(index, message.length));

  if (consoleOutput) {
    if (objTree) {
      consoleOutput.push(util.inspect(objTree, {colors: true, depth: null}));
    }
    console.log(consoleOutput.join(''));
  }

  if (logFileOutput) {
    if (objTree) {
      logFileOutput.push(util.inspect(objTree, {colors: false, depth: null}));
    }
    logFileOutput.push('\n');
    fs.appendFileSync(logFilePath!, logFileOutput.join(''), 'utf-8');
  }

  function write(text: string, colorizer?: (text: string) => string) {
    if (consoleOutput) {
      consoleOutput.push(colorizer ? colorizer(text) : text);
    }
    if (logFileOutput) {
      logFileOutput.push(text);
    }
  }
}

export const log: Logger = Object.assign(
  (message: string, objTree?: any) => writeLog('info', message, objTree),
  {
    verbose: (message: string, objTree?: any) =>
      writeLog('info', message, objTree),
    warning: (message: string, objTree?: any) =>
      writeLog('warning', message, objTree),
    error: (message: string, objTree?: any) =>
      writeLog('error', message, objTree),
    init: (options: LoggerOptions) => {
      log.options = {...options, quiet: options.quiet ?? false};
      if (options.logFile) {
        fs.writeFileSync(
          path.join(process.cwd(), options.logFile),
          '',
          'utf-8',
        );
      }
    },
    options: {},
  },
);
