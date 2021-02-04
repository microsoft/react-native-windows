/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

//
// The logger used by the Doxysaurus project.
//
// The goal is make the logging looking simpler in the code.
// - 'Warning:' prefixes are rendered in yellow color.
// - 'Error:' prefixes are rendered in red color.
// - Text in square brackets [] is rendered in green.
// - Text in curly brackets {} is rendered in bright cyan.
// - Repeat [] or {} brackets to render them. E.g. {{, ]], }}, ]].
// - Provide the second objTree parameter to dump a JavaScrip object.
// - Setting `log.quiet = true` disables console output.
//

import chalk from 'chalk';
import fs from 'fs';
import path from 'path';
import util from 'util';

export interface LoggerOptions {
  quiet?: boolean;
  logFile?: string;
}

export interface Logger {
  (message: string, obj?: any): void;
  init: (options: LoggerOptions) => void;
  options: LoggerOptions;
}

export const log: Logger = Object.assign(
  (message: string, objTree?: any) => {
    const warning = 'Warning:';
    const error = 'Error:';
    const hasError = message.startsWith(error);

    const logFilePath = log.options.logFile
      ? path.join(process.cwd(), log.options.logFile)
      : undefined;
    const consoleOutput: string[] | undefined =
      hasError || !log.options.quiet ? [] : undefined;
    const logFileOutput: string[] | undefined = logFilePath ? [] : undefined;

    let index = 0;
    if (hasError) {
      write(error, chalk.redBright);
      index += error.length;
    } else if (message.startsWith(warning)) {
      write(warning, chalk.yellow);
      index += warning.length;
    }

    const openBrackets = '[{';
    const closeBrackets = ']}';
    let bracketIndex = -1;
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
      if (typeof objTree !== 'undefined') {
        consoleOutput.push(util.inspect(objTree, {colors: true, depth: null}));
      }
      console.log(consoleOutput.join(''));
    }

    if (logFileOutput) {
      if (typeof objTree !== 'undefined') {
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
  },
  {
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
